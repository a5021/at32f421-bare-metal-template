/**
 * AT32F421 USART Configuration
 *
 * Purpose: Configure USART for serial communication with printf support
 * Features: Precise baud rate calculation with automatic BRR calculation
 * Performance: Optimized for centralized system clock operation
 * Usage: Define USART_BAUD_RATE or use default (115200)
 *
 * Configuration Options:
 * • USART_BAUD_RATE: Baud rate in bps (default: 115200)
 *
 * Note: USART clocks must be enabled before calling usart_config()
 *       Clock frequency automatically sourced from crm.h
 *
 * Key Features:
 * ✓ Direct register access (no bitfield structures)
 * ✓ Precise baud rate calculation with rounding
 * ✓ Automatic clock frequency adjustment (120MHz/125MHz)
 * ✓ Baud rate error calculation and validation
 * ✓ No standard library dependencies
 *
 * Example Usage:
 *   #define USART_BAUD_RATE 9600  // 9600 baud rate
 *   crm_config();                 // Enable clocks first
 *   usart_config();               // Initialize USART
 *   usart_puts("Hello World!\n"); // Print to USART
 */

#ifndef USART_H
#define USART_H

#include "at32f421.h"
#include "crm.h"

/* Configuration macros */
#ifndef USART_BAUD_RATE
  #define USART_BAUD_RATE             115200U
#endif

/* USART BRR calculation using centralized clock from crm.h */
#define USART_BRR_VALUE             ((USART1_CLOCK_HZ + (USART_BAUD_RATE / 2)) / USART_BAUD_RATE)

/* Calculate actual achieved baud rate for validation */
#define USART_ACTUAL_BAUD           (USART1_CLOCK_HZ / USART_BRR_VALUE)

/* Calculate baud rate error in parts per million */
#define USART_ERROR_PPM             (((USART_ACTUAL_BAUD > USART_BAUD_RATE) ? \
                                      (USART_ACTUAL_BAUD - USART_BAUD_RATE) : \
                                      (USART_BAUD_RATE - USART_ACTUAL_BAUD)) * 1000000U / USART_BAUD_RATE)

/* Validate baud rate error is acceptable (< 2.5%) */
#if USART_ERROR_PPM > 25000U
  #warning "USART baud rate error exceeds 2.5%, consider different baud rate"
#endif

/* USART CTRL1 register bit definitions */
#define USART_CTRL1_UEN_Pos         13
#define USART_CTRL1_UEN             (0x1U << USART_CTRL1_UEN_Pos)

#define USART_CTRL1_TEN_Pos         3
#define USART_CTRL1_TEN             (0x1U << USART_CTRL1_TEN_Pos)

#define USART_CTRL1_REN_Pos         2
#define USART_CTRL1_REN             (0x1U << USART_CTRL1_REN_Pos)

/* USART STS register bit definitions */
#define USART_STS_TDC_Pos           6
#define USART_STS_TDC               (0x1U << USART_STS_TDC_Pos)

#define USART_STS_TDBE_Pos          7
#define USART_STS_TDBE              (0x1U << USART_STS_TDBE_Pos)

#define USART_STS_RDBF_Pos          5
#define USART_STS_RDBF              (0x1U << USART_STS_RDBF_Pos)

/**
 * @brief Configure USART using direct register access with precise baud rate
 */
void usart_config(void);

/**
 * @brief Send a single character via USART
 * @param ch Character to send
 */
void usart_putchar(char ch);

/**
 * @brief Send a null-terminated string via USART
 * @param str String to send
 */
void usart_puts(const char* str);

/**
 * @brief Send an unsigned integer as decimal via USART
 * @param value Unsigned integer to send
 */
void usart_put_uint(uint32_t value);

#endif /* USART_H */
