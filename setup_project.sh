#!/bin/bash

# Enhanced download function with directory parameter
download_files() {
    local base_url="$1"
    local output_dir="$2"
    shift 2
    local files=("$@")

    # Create output directory if it doesn't exist
    mkdir -p "$output_dir"

    echo "========================================"
    echo "Downloading to: $output_dir/"
    echo "Source: $base_url"
    echo "Files: ${#files[@]}"
    echo "----------------------------------------"

    local success_count=0
    for file in "${files[@]}"; do
        echo -n "Downloading $file... "
        if wget --no-check-certificate --user-agent="Mozilla/5.0" -q -O "$output_dir/$file" "${base_url}/${file}"; then
            echo "✓ Success"
            ((success_count++))
        else
            echo "✗ Failed"
        fi
    done

    echo "----------------------------------------"
    echo "Successfully downloaded: $success_count/${#files[@]} files"
    echo ""
}

# Main script
BASE_URL="https://raw.githubusercontent.com/ArteryTek/AT32F421_Firmware_Library/refs/heads/master/libraries"

# CMSIS files
CMSIS_FILES=(
    "cmsis_armcc.h"
    "cmsis_armclang.h"
    "cmsis_armclang_ltm.h"
    "cmsis_gcc.h"
    "cmsis_compiler.h"
    "cmsis_version.h"
    "core_cm4.h"
    "mpu_armv7.h"
)

# Driver files
DRIVER_FILES=(
    "at32f421_crm.h"
    "at32f421_flash.h"
    "at32f421_tmr.h"
    "at32f421_def.h"
    "at32f421_gpio.h"
    "at32f421_usart.h"
)

DEVICE_FILES=(
    "at32f421.h"
    "system_at32f421.h"
    "at32f421_conf_template.h"
)


echo "AT32F421 Library Downloader"
echo "==========================="

# Download files to respective directories
download_files "$BASE_URL/cmsis/cm4/core_support" "inc" "${CMSIS_FILES[@]}"
download_files "$BASE_URL/drivers/inc" "inc" "${DRIVER_FILES[@]}"
download_files "$BASE_URL/cmsis/cm4/device_support" "inc" "${DEVICE_FILES[@]}"
download_files "$BASE_URL/cmsis/cm4/device_support" "." "system_at32f421.c"
download_files "$BASE_URL/cmsis/cm4/device_support/startup/gcc" "." "startup_at32f421.s"
download_files "$BASE_URL/cmsis/cm4/device_support/startup/gcc/linker" "." "AT32F421x8_FLASH.ld"


# --- Post-Download Configuration ---
echo "Running Post-Download Configuration"
echo "==================================="

set -euo pipefail

# Create at32f421_conf.h from template
CONF_TEMPLATE="inc/at32f421_conf_template.h"
CONF_FILE="inc/at32f421_conf.h"

if [[ -f "$CONF_TEMPLATE" ]]; then
    echo "Creating $CONF_FILE from template..."

    # List of enabled modules
    ENABLED_MODULES=(
        "CRM_MODULE_ENABLED"
        "TMR_MODULE_ENABLED"
        "USART_MODULE_ENABLED"
        "GPIO_MODULE_ENABLED"
        "FLASH_MODULE_ENABLED"
    )

    cp "$CONF_TEMPLATE" "$CONF_FILE"

    # Comment out all #define except the enabled ones
    while IFS= read -r line; do
        if [[ "$line" =~ ^[[:space:]]*#define[[:space:]]+([A-Z0-9_]+_MODULE_ENABLED) ]]; then
            macro="${BASH_REMATCH[1]}"
            if [[ ! " ${ENABLED_MODULES[*]} " =~ " $macro " ]]; then
                sed -i "s|^$line|// $line|" "$CONF_FILE"
            fi
        fi
    done < "$CONF_TEMPLATE"

    echo "✓ Created $CONF_FILE with only required modules enabled."
else
    echo "✗ Template $CONF_TEMPLATE not found. Skipping configuration file generation."
fi


# --- Disable __libc_init_array in startup file ---
STARTUP_FILE="startup_at32f421.s"

if [[ -f "$STARTUP_FILE" ]]; then
    echo "Patching $STARTUP_FILE (disabling __libc_init_array)..."

    # Create a backup
    cp "$STARTUP_FILE" "${STARTUP_FILE}.bak"

    # sed expression: replace line "bl __libc_init_array" with "@ bl __libc_init_array"
    if sed --version >/dev/null 2>&1; then
        # GNU sed
        sed -E -i 's/^([[:space:]]*)bl[[:space:]]+__libc_init_array/\1@ bl __libc_init_array/' "$STARTUP_FILE"
    else
        # BSD/macOS sed
        sed -E -i '' 's/^([[:space:]]*)bl[[:space:]]+__libc_init_array/\1@ bl __libc_init_array/' "$STARTUP_FILE"
    fi

    echo "✓ Patched $STARTUP_FILE (line commented out)."
else
    echo "✗ Startup file $STARTUP_FILE not found. Skipping patch."
fi
