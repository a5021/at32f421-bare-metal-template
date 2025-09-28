##############################################################################
# AT32F421 Bare Metal Project Makefile
# Clean build without warnings
##############################################################################

# Project name
PROJECT = at32f421_project

# Target microcontroller
MCU = AT32F421F8P7

# Build directory
BUILD_DIR = build

# Source directories
SRC_DIRS = .

# Include directories  
INC_DIRS = . ./inc

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
GDB = $(PREFIX)gdb

# MCU specific flags - AT32F421 is Cortex-M4 WITHOUT FPU
CPU = -mcpu=cortex-m4
FLOAT-ABI = -mfloat-abi=soft
MCU_FLAGS = $(CPU) -mthumb $(FLOAT-ABI)

# Defines - removed __FPU_PRESENT since it's already defined in at32f421.h
DEFINES = -D$(MCU) -DUSE_STDPERIPH_DRIVER

# Include paths
INCLUDES = $(addprefix -I,$(INC_DIRS))

# C flags
CFLAGS = $(MCU_FLAGS) $(DEFINES) $(INCLUDES)
CFLAGS += -Wall -Wextra -Wstrict-prototypes -Wundef
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -fstack-usage -MMD -MP

# Debug vs Release
ifeq ($(DEBUG), 1)
    CFLAGS += -g3 -Og -DDEBUG
else
    CFLAGS += -Os -DNDEBUG
endif

# Assembly flags
ASFLAGS = $(MCU_FLAGS) $(DEFINES) $(INCLUDES)
ASFLAGS += -Wall -fdata-sections -ffunction-sections

# Linker flags - configured for bare metal without stdlib warnings
LDFLAGS = $(MCU_FLAGS)
LDFLAGS += -nostartfiles -nostdlib
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += -lgcc

# Linker script
LDSCRIPT = AT32F421x8_FLASH.ld
LDFLAGS += -T$(LDSCRIPT)

##############################################################################
# Source Files
##############################################################################

# C source files (automatically found)
C_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

# Assembly source files - startup file in current directory
ASM_SOURCES = startup_at32f421.s

# Object files
C_OBJECTS = $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o))
ASM_OBJECTS = $(addprefix $(BUILD_DIR)/,$(ASM_SOURCES:.s=.o))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS = $(C_OBJECTS:.o=.d)

##############################################################################
# Build Rules
##############################################################################

# Default target
all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).bin size

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src
	mkdir -p $(BUILD_DIR)/drivers  
	mkdir -p $(BUILD_DIR)/CMSIS

# Compile C source files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

# Compile assembly source files
$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	@echo "AS $<"
	@$(AS) -c $(ASFLAGS) $< -o $@

# Link executable
$(BUILD_DIR)/$(PROJECT).elf: $(OBJECTS) $(LDSCRIPT) | $(BUILD_DIR)
	@echo "LD $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Generate hex file
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "HEX $@"
	@$(CP) -O ihex $< $@

# Generate binary file  
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "BIN $@"
	@$(CP) -O binary -S $< $@

# Print size information
size: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Size information:"
	@$(SZ) $<

##############################################################################
# Utility Targets
##############################################################################

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Flash using OpenOCD (adjust for your debugger)
flash: $(BUILD_DIR)/$(PROJECT).hex
	openocd -f interface/stlink.cfg -f target/at32f4xx.cfg -c "program $(BUILD_DIR)/$(PROJECT).hex verify reset exit"

# Debug using GDB + OpenOCD
debug: $(BUILD_DIR)/$(PROJECT).elf
	$(GDB) -ex "target extended-remote :3333" -ex "load" -ex "break main" -ex "continue" $<

# Show memory usage
memory: $(BUILD_DIR)/$(PROJECT).elf
	@$(SZ) -A -d $<
	@$(SZ) -B -d $<

# Generate assembly listing
list: $(BUILD_DIR)/$(PROJECT).elf
	@$(PREFIX)objdump -h -S $< > $(BUILD_DIR)/$(PROJECT).list

# Show all symbols
symbols: $(BUILD_DIR)/$(PROJECT).elf  
	@$(PREFIX)nm -S -s --size-sort $< > $(BUILD_DIR)/$(PROJECT).sym

# Disassemble
disasm: $(BUILD_DIR)/$(PROJECT).elf
	@$(PREFIX)objdump -d $< > $(BUILD_DIR)/$(PROJECT).disasm

##############################################################################
# Special Targets
##############################################################################

# Help
help:
	@echo "Available targets:"
	@echo "  all        - Build project (default)"
	@echo "  clean      - Remove build files"  
	@echo "  flash      - Flash using OpenOCD"
	@echo "  debug      - Start GDB debug session"
	@echo "  memory     - Show memory usage"
	@echo "  list       - Generate assembly listing"
	@echo "  symbols    - Show all symbols"
	@echo "  disasm     - Generate disassembly"
	@echo ""
	@echo "Options:"
	@echo "  DEBUG=1    - Build with debug symbols"

# Phony targets
.PHONY: all clean flash debug memory list symbols disasm help size

# Include dependency files
-include $(DEPS)
