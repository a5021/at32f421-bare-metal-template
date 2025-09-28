/**
 * @file   gpio.c
 * @brief  AT32F421 GPIO Configuration Implementation
 *
 * @note   This file contains the highly-optimized initialization routine
 *         for GPIO Port A, based on direct register assignment. It assumes
 *         it is called once, immediately after a hardware reset.
 */
#include "gpio.h"

/**
 * @brief  Configures all GPIO pins on port A for their specific functions.
 */
void gpio_config(void) {
    // --- GPIOA Mode Register (CFGR) ---
    // Direct write. Sets application and SWD pins to Alternate Function,
    // and all other pins on the port to Analog Mode for low power.
    GPIOA->cfgr = (0xFFFFFFFF
                   & ~(GPIO_PA4_CFGR_Msk | GPIO_PA9_CFGR_Msk | GPIO_PA10_CFGR_Msk
                       | GPIO_PA13_CFGR_Msk | GPIO_PA14_CFGR_Msk))
                   | (GPIO_PA4_CFGR_MUX | GPIO_PA9_CFGR_MUX | GPIO_PA10_CFGR_MUX
                      | GPIO_PA13_CFGR_MUX | GPIO_PA14_CFGR_MUX);

    // --- GPIOA Output Speed Register (ODRVR) ---
    // Direct write. Sets low speed for application output pins.
    GPIOA->odrvr = GPIO_PA4_ODRVR_LOW | GPIO_PA9_ODRVR_LOW;

    // --- GPIOA Pull-up/Pull-down Register (PULLR) ---
    // Direct write. Enforces required pulls for SWD and USART RX.
    GPIOA->pull = GPIO_PA10_PULL_UP | GPIO_PA13_PULL_UP | GPIO_PA14_PULL_DOWN;

    // --- GPIOA Alternate Function Multiplexer Registers (MUXL/MUXH) ---
    // Direct write. Configures the specific AF number for each required pin.
    GPIOA->muxl = GPIO_PA4_MUXL_AF4;
    GPIOA->muxh = GPIO_PA9_MUXH_AF1 | GPIO_PA10_MUXH_AF1
                  | GPIO_PA13_MUXH_AF0 | GPIO_PA14_MUXH_AF0;
}
