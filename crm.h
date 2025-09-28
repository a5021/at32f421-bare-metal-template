/**
 * AT32F421 CRM (Clock and Reset Management) Configuration
 * 
 * Purpose: Configure AT32F421 CRM module for system clock and peripheral clocks
 * Features: Supports both internal HICK and external HEXT crystal sources
 * Performance: Optimized with Flash prefetch buffers and auto-step mode
 * Usage: Define HEXT_FREQUENCY (4,8,12,16,20,25) for external crystal or leave
 *        undefined to use internal 4MHz HICK oscillator
 * 
 * Configuration Options:
 *   • HICK (default): 4MHz × 30 = 120MHz
 *   • HEXT 4MHz:      4MHz × 30 = 120MHz
 *   • HEXT 8MHz:      8MHz × 15 = 120MHz
 *   • HEXT 12MHz:     12MHz × 10 = 120MHz
 *   • HEXT 16MHz:     16MHz/2 × 15 = 120MHz
 *   • HEXT 20MHz:     20MHz/2 × 12 = 120MHz
 *   • HEXT 25MHz:     25MHz/2 × 10 = 125MHz (Warning: Not exactly 120MHz)
 * 
 * Key Features:
 *   ✓ System clock configuration (up to 120MHz)
 *   ✓ Single function to enable all used peripheral clocks
 *   ✓ Automatic PLL multiplication calculation
 *   ✓ Flash wait cycles optimized for high frequency
 *   ✓ Dual prefetch buffers enabled for maximum performance
 *   ✓ Auto-step mode for smooth clock transitions
 *   ✓ Robust timeout handling with error reporting
 *   ✓ Compile-time configuration validation
 *   ✓ Centralized system clock definitions for all modules
 * 
 * Example Usage:
 *   #define HEXT_FREQUENCY 8    // Use 8MHz external crystal
 *   
 *   if (crm_config() != CRM_OK) {
 *       // Handle clock configuration error
 *       while(1);  // Error: blink LED, reset, etc.
 *   }
 */

#ifndef CRM_H
#define CRM_H

#include "at32f421.h"

/* Configuration macro - define external crystal frequency to use HEXT */
// #define HEXT_FREQUENCY    8   // External crystal frequency in MHz (uncomment to use HEXT)

/* Timeout values for clock operations - avoid naming conflicts with HAL */
#define CRM_HEXT_TIMEOUT            50000U
#define CRM_PLL_TIMEOUT             50000U
#define CRM_SWITCH_TIMEOUT          50000U

/*******************************************************************************
 * CRM Status/Error Codes
 ******************************************************************************/

/**
 * @brief CRM operation result codes
 */
typedef enum {
    CRM_OK = 0,                     /*!< Operation completed successfully */
    CRM_ERR_HEXT_TIMEOUT,           /*!< External crystal failed to stabilize */
    CRM_ERR_PLL_TIMEOUT,            /*!< PLL failed to lock */
    CRM_ERR_SWITCH_TIMEOUT          /*!< System clock switch failed */
} crm_status_t;

/*******************************************************************************
 * System Clock Definitions - Single Source of Truth
 ******************************************************************************/

/* System clock frequency - depends on crystal configuration */
#if defined(HEXT_FREQUENCY) && (HEXT_FREQUENCY == 25)
    #define SYSTEM_CLOCK_HZ         125000000U  /* 25MHz crystal case */
#else
    #define SYSTEM_CLOCK_HZ         120000000U  /* All other cases: HICK, 4/8/12/16/20MHz crystals */
#endif

/* Derived bus frequencies (all running at system clock speed) */
#define AHB_CLOCK_HZ                SYSTEM_CLOCK_HZ
#define APB1_CLOCK_HZ               SYSTEM_CLOCK_HZ
#define APB2_CLOCK_HZ               SYSTEM_CLOCK_HZ

/* Peripheral clock sources for modules to use */
#define TIMER_CLOCK_HZ              APB1_CLOCK_HZ
#define USART1_CLOCK_HZ             APB2_CLOCK_HZ
#define GPIO_CLOCK_HZ               AHB_CLOCK_HZ

/*******************************************************************************
 * CRM CFG Register Bit Definitions
 ******************************************************************************/

/* System Clock Selection */
#define CRM_CFG_SCLKSEL_Pos         0
#define CRM_CFG_SCLKSEL_Msk         (0x3U << CRM_CFG_SCLKSEL_Pos)
#define CRM_CFG_SCLKSEL_HICK        (0x0U << CRM_CFG_SCLKSEL_Pos)
#define CRM_CFG_SCLKSEL_HEXT        (0x1U << CRM_CFG_SCLKSEL_Pos)
#define CRM_CFG_SCLKSEL_PLL         (0x2U << CRM_CFG_SCLKSEL_Pos)

/* System Clock Status */
#define CRM_CFG_SCLKSTS_Pos         2
#define CRM_CFG_SCLKSTS_Msk         (0x3U << CRM_CFG_SCLKSTS_Pos)
#define CRM_CFG_SCLKSTS_PLL         (0x2U << CRM_CFG_SCLKSTS_Pos)

/* AHB Clock Divider */
#define CRM_CFG_AHBDIV_Pos          4
#define CRM_CFG_AHBDIV_Msk          (0xFU << CRM_CFG_AHBDIV_Pos)
#define CRM_CFG_AHBDIV_1            (0x0U << CRM_CFG_AHBDIV_Pos)

/* APB1 Clock Divider */
#define CRM_CFG_APB1DIV_Pos         8
#define CRM_CFG_APB1DIV_Msk         (0x7U << CRM_CFG_APB1DIV_Pos)
#define CRM_CFG_APB1DIV_1           (0x0U << CRM_CFG_APB1DIV_Pos)

/* APB2 Clock Divider */
#define CRM_CFG_APB2DIV_Pos         11
#define CRM_CFG_APB2DIV_Msk         (0x7U << CRM_CFG_APB2DIV_Pos)
#define CRM_CFG_APB2DIV_1           (0x0U << CRM_CFG_APB2DIV_Pos)

/* PLL Reference Clock Source */
#define CRM_CFG_PLLRCS_Pos          16
#define CRM_CFG_PLLRCS_Msk          (0x1U << CRM_CFG_PLLRCS_Pos)
#define CRM_CFG_PLLRCS_HICK         (0x0U << CRM_CFG_PLLRCS_Pos)
#define CRM_CFG_PLLRCS_HEXT         (0x1U << CRM_CFG_PLLRCS_Pos)

/* HEXT Clock Divider for PLL */
#define CRM_CFG_PLLHEXTDIV_Pos      17
#define CRM_CFG_PLLHEXTDIV_Msk      (0x1U << CRM_CFG_PLLHEXTDIV_Pos)
#define CRM_CFG_PLLHEXTDIV_1        (0x0U << CRM_CFG_PLLHEXTDIV_Pos)
#define CRM_CFG_PLLHEXTDIV_2        (0x1U << CRM_CFG_PLLHEXTDIV_Pos)

/* PLL Multiplication Factor (split across two bit fields) */
#define CRM_CFG_PLLMULT_L_Pos       18      /* Low bits [21:18] - 4 bits */
#define CRM_CFG_PLLMULT_L_Msk       (0xFU << CRM_CFG_PLLMULT_L_Pos)
#define CRM_CFG_PLLMULT_H_Pos       29      /* High bits [30:29] - 2 bits */
#define CRM_CFG_PLLMULT_H_Msk       (0x3U << CRM_CFG_PLLMULT_H_Pos)

/*******************************************************************************
 * CRM CTRL Register Bit Definitions
 ******************************************************************************/

/* HEXT Control */
#define CRM_CTRL_HEXTEN_Pos         16
#define CRM_CTRL_HEXTEN             (0x1U << CRM_CTRL_HEXTEN_Pos)
#define CRM_CTRL_HEXTSTBL_Pos       17
#define CRM_CTRL_HEXTSTBL           (0x1U << CRM_CTRL_HEXTSTBL_Pos)

/* PLL Control */
#define CRM_CTRL_PLLEN_Pos          24
#define CRM_CTRL_PLLEN              (0x1U << CRM_CTRL_PLLEN_Pos)
#define CRM_CTRL_PLLSTBL_Pos        25
#define CRM_CTRL_PLLSTBL            (0x1U << CRM_CTRL_PLLSTBL_Pos)

/*******************************************************************************
 * CRM MISC2 Register Bit Definitions
 ******************************************************************************/

/* Auto-step Mode Control */
#define CRM_MISC2_AUTO_STEP_EN_Pos  4
#define CRM_MISC2_AUTO_STEP_EN_Msk  (0x3U << CRM_MISC2_AUTO_STEP_EN_Pos)
#define CRM_MISC2_AUTO_STEP_EN      (0x3U << CRM_MISC2_AUTO_STEP_EN_Pos)

/*******************************************************************************
 * CRM Peripheral Clock Enable Register Bit Definitions
 ******************************************************************************/

/* AHB Peripheral Clock Enable */
#define CRM_AHBEN_GPIOAEN_Pos       17
#define CRM_AHBEN_GPIOAEN           (0x1U << CRM_AHBEN_GPIOAEN_Pos)

/* APB1 Peripheral Clock Enable */
#define CRM_APB1EN_TMR14EN_Pos      8
#define CRM_APB1EN_TMR14EN          (0x1U << CRM_APB1EN_TMR14EN_Pos)

/* APB2 Peripheral Clock Enable */
#define CRM_APB2EN_USART1EN_Pos     14
#define CRM_APB2EN_USART1EN         (0x1U << CRM_APB2EN_USART1EN_Pos)

/*******************************************************************************
 * Flash PSR Register Bit Definitions
 ******************************************************************************/

/* Flash Wait Cycles */
#define FLASH_PSR_WTCYC_Pos         0
#define FLASH_PSR_WTCYC_Msk         (0x7U << FLASH_PSR_WTCYC_Pos)
#define FLASH_PSR_WTCYC_3           (0x3U << FLASH_PSR_WTCYC_Pos)

/* Flash Prefetch Control */
#define FLASH_PSR_PFT_EN_Pos        4
#define FLASH_PSR_PFT_EN            (0x1U << FLASH_PSR_PFT_EN_Pos)     /* Main prefetch enable */
#define FLASH_PSR_PFT_EN2_Pos       6
#define FLASH_PSR_PFT_EN2           (0x1U << FLASH_PSR_PFT_EN2_Pos)    /* Prefetch buffer block 2 enable */

/*******************************************************************************
 * PLL Configuration Based on Clock Source
 ******************************************************************************/

#ifdef HEXT_FREQUENCY
  /* HEXT PLL configurations for different crystal frequencies targeting ~120MHz */
  #if HEXT_FREQUENCY == 4
    /* 4MHz HEXT -> 120MHz: 4MHz × 30 = 120MHz */
    #define PLL_MULT_FACTOR         30
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_1
  #elif HEXT_FREQUENCY == 8
    /* 8MHz HEXT -> 120MHz: 8MHz × 15 = 120MHz */
    #define PLL_MULT_FACTOR         15
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_1
  #elif HEXT_FREQUENCY == 12
    /* 12MHz HEXT -> 120MHz: 12MHz × 10 = 120MHz */
    #define PLL_MULT_FACTOR         10
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_1
  #elif HEXT_FREQUENCY == 16
    /* 16MHz HEXT/2 -> 120MHz: 8MHz × 15 = 120MHz */
    #define PLL_MULT_FACTOR         15
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_2
  #elif HEXT_FREQUENCY == 20
    /* 20MHz HEXT/2 -> 120MHz: 10MHz × 12 = 120MHz */
    #define PLL_MULT_FACTOR         12
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_2
  #elif HEXT_FREQUENCY == 25
    /* 25MHz HEXT/2 -> 125MHz: 12.5MHz × 10 = 125MHz */
    #define PLL_MULT_FACTOR         10
    #define CRM_CFG_PLLRCS_SEL      CRM_CFG_PLLRCS_HEXT
    #define CRM_CFG_PLLHEXTDIV_SEL  CRM_CFG_PLLHEXTDIV_2
    #warning "25MHz crystal results in 125MHz system clock (not exactly 120MHz)"
  #else
    #error "Unsupported HEXT frequency. Supported: 4, 8, 12, 16, 20, 25 MHz"
  #endif

  /* Calculate PLL multiplication factor encoding for HEXT */
  #define CRM_CFG_PLLMULT_L      (((PLL_MULT_FACTOR - 2) & 0x0F) << CRM_CFG_PLLMULT_L_Pos)
  #define CRM_CFG_PLLMULT_H      (((PLL_MULT_FACTOR - 2) >> 4) << CRM_CFG_PLLMULT_H_Pos)
#else
  /* HICK PLL configuration: 4MHz × 30 = 120MHz */
  #define PLL_MULT_FACTOR        30
  #define CRM_CFG_PLLMULT_L      (0xEU << CRM_CFG_PLLMULT_L_Pos)  /* Low 4 bits: 1110 */
  #define CRM_CFG_PLLMULT_H      (0x1U << CRM_CFG_PLLMULT_H_Pos)  /* High 2 bits: 01 */
  /* Note: PLLRCS = 0 (HICK) and PLLHEXTDIV = 0 by default, no need to OR with 0 */
#endif

/*******************************************************************************
 * Function Declarations
 ******************************************************************************/

/**
 * @brief Configure CRM system clock and enable all used peripheral clocks
 * 
 * System Clock Configuration:
 * - System clock target = SYSTEM_CLOCK_HZ (120MHz or 125MHz for 25MHz crystal)
 * - System clock source = PLL
 * - AHB divider         = 1 (HCLK = SCLK)
 * - APB2 divider        = 1 (PCLK2 = SCLK)
 * - APB1 divider        = 1 (PCLK1 = SCLK)
 * - Auto-step mode      = enabled (recommended for >108MHz)
 * - Flash wait cycles   = 3
 * - Flash prefetch      = enabled (both buffers)
 * 
 * Enabled Peripheral Clocks:
 * - GPIOA  (for PA4, PA9, PA10)
 * - TMR14  (for PWM output)
 * - USART1 (for serial communication)
 * 
 * @return CRM_OK if successful, or specific error code if failed
 * @retval CRM_OK                Configuration successful
 * @retval CRM_ERR_HEXT_TIMEOUT  External crystal failed to stabilize
 * @retval CRM_ERR_PLL_TIMEOUT   PLL failed to lock
 * @retval CRM_ERR_SWITCH_TIMEOUT System clock switch failed
 */
crm_status_t crm_config(void);

#endif /* CRM_H */
