/**
 * AT32F421 GPIO CONFIGURATION MODULE
 * 
 * Purpose: Single function to configure all GPIO pins used in the project
 * Features: Direct register access with named constants for maximum efficiency
 * Usage: Call gpio_config() once after enabling GPIOA clock
 * 
 * Configured Pins:
 *   • PA4  - TMR14_CH1 (AF4) - PWM output
 *   • PA9  - USART1_TX (AF1) - Serial transmit
 *   • PA10 - USART1_RX (AF1) - Serial receive
 *   • PA13 - SWDIO (AF0)     - Preserved for debugging
 *   • PA14 - SWCLK (AF0)     - Preserved for debugging
 * 
 * Note: All other pins on Port A are set to Analog mode for lowest power consumption.
 *       GPIO clocks must be enabled before calling gpio_config().
 */

#ifndef GPIO_H
#define GPIO_H

#include "at32f421.h"

// --- Generic GPIO Configuration Values ---
#define GPIO_MODE_MUX         0x2U // Alternate Function
#define GPIO_OSPEED_LOW       0x0U
#define GPIO_PULL_UP          0x1U
#define GPIO_PULL_DOWN        0x2U
#define GPIO_AF0_SYSTEM       0x0U // JTAG/SWD
#define GPIO_AF1_USART        0x1U
#define GPIO_AF4_TIMER        0x4U

// --- Pin-Specific Configuration Constants for GPIOA ---
// PA4 (TMR14_CH1)
#define GPIO_PA4_CFGR_Pos     (4 * 2)
#define GPIO_PA4_CFGR_Msk     (0x3U << GPIO_PA4_CFGR_Pos)
#define GPIO_PA4_CFGR_MUX     (GPIO_MODE_MUX << GPIO_PA4_CFGR_Pos)
#define GPIO_PA4_ODRVR_Pos    (4 * 2)
#define GPIO_PA4_ODRVR_LOW    (GPIO_OSPEED_LOW << GPIO_PA4_ODRVR_Pos)
#define GPIO_PA4_MUXL_Pos     (4 * 4)
#define GPIO_PA4_MUXL_AF4     (GPIO_AF4_TIMER << GPIO_PA4_MUXL_Pos)

// PA9 (USART1_TX)
#define GPIO_PA9_CFGR_Pos     (9 * 2)
#define GPIO_PA9_CFGR_Msk     (0x3U << GPIO_PA9_CFGR_Pos)
#define GPIO_PA9_CFGR_MUX     (GPIO_MODE_MUX << GPIO_PA9_CFGR_Pos)
#define GPIO_PA9_ODRVR_Pos    (9 * 2)
#define GPIO_PA9_ODRVR_LOW    (GPIO_OSPEED_LOW << GPIO_PA9_ODRVR_Pos)
#define GPIO_PA9_MUXH_Pos     ((9 - 8) * 4)
#define GPIO_PA9_MUXH_AF1     (GPIO_AF1_USART << GPIO_PA9_MUXH_Pos)

// PA10 (USART1_RX)
#define GPIO_PA10_CFGR_Pos    (10 * 2)
#define GPIO_PA10_CFGR_Msk    (0x3U << GPIO_PA10_CFGR_Pos)
#define GPIO_PA10_CFGR_MUX    (GPIO_MODE_MUX << GPIO_PA10_CFGR_Pos)
#define GPIO_PA10_PULL_Pos    (10 * 2)
#define GPIO_PA10_PULL_UP     (GPIO_PULL_UP << GPIO_PA10_PULL_Pos)
#define GPIO_PA10_MUXH_Pos    ((10 - 8) * 4)
#define GPIO_PA10_MUXH_AF1    (GPIO_AF1_USART << GPIO_PA10_MUXH_Pos)

// PA13 (SWDIO)
#define GPIO_PA13_CFGR_Pos    (13 * 2)
#define GPIO_PA13_CFGR_Msk    (0x3U << GPIO_PA13_CFGR_Pos)
#define GPIO_PA13_CFGR_MUX    (GPIO_MODE_MUX << GPIO_PA13_CFGR_Pos)
#define GPIO_PA13_PULL_Pos    (13 * 2)
#define GPIO_PA13_PULL_UP     (GPIO_PULL_UP << GPIO_PA13_PULL_Pos)
#define GPIO_PA13_MUXH_Pos    ((13 - 8) * 4)
#define GPIO_PA13_MUXH_AF0    (GPIO_AF0_SYSTEM << GPIO_PA13_MUXH_Pos)

// PA14 (SWCLK)
#define GPIO_PA14_CFGR_Pos    (14 * 2)
#define GPIO_PA14_CFGR_Msk    (0x3U << GPIO_PA14_CFGR_Pos)
#define GPIO_PA14_CFGR_MUX    (GPIO_MODE_MUX << GPIO_PA14_CFGR_Pos)
#define GPIO_PA14_PULL_Pos    (14 * 2)
#define GPIO_PA14_PULL_DOWN   (GPIO_PULL_DOWN << GPIO_PA14_PULL_Pos)
#define GPIO_PA14_MUXH_Pos    ((14 - 8) * 4)
#define GPIO_PA14_MUXH_AF0    (GPIO_AF0_SYSTEM << GPIO_PA14_MUXH_Pos)

/**
 * @brief  Configures all necessary GPIO pins on port A.
 */
void gpio_config(void);

#endif // GPIO_H

