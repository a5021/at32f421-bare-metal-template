/**
 * @file timer.h
 * @brief TMR14 Configuration for PWM and Event Generation
 * 
 * Configures Timer 14 for PWM output with centralized system clock configuration.
 * All timing calculations are based on crm.h definitions to ensure consistency
 * across the entire project and automatic handling of different system frequencies.
 * 
 * Features:
 * - Fixed-frequency PWM signal generation
 * - Active-low polarity PWM output
 * - Immediate register updates via force update event
 * - Update interrupt for WFE event generation
 * - Automatic clock frequency adjustment (120MHz/125MHz)
 * 
 * All bit definitions follow CMSIS style for consistency.
 */

#ifndef TIMER_H
#define TIMER_H

#include "at32f421.h"
#include "crm.h"

/*******************************************************************************
 * PWM Configuration (uses centralized system clock from crm.h)
 ******************************************************************************/

/* PWM configuration - can be overridden before including this header */
#ifndef PWM_FREQUENCY_HZ
  #define PWM_FREQUENCY_HZ          1U          /* Default: 1Hz PWM frequency */
#endif

#ifndef PWM_DUTY_RATIO
  #define PWM_DUTY_RATIO            10U         /* Default: 10% duty cycle */
#endif

/* PWM timing base - 10kHz timer frequency for good resolution */
#define PWM_TIMER_FREQ_HZ           10000U

/* PWM prescaler calculation based on timer clock from crm.h */
#define PWM_PRESCALER               ((TIMER_CLOCK_HZ / PWM_TIMER_FREQ_HZ) - 1U)

/* PWM period calculation */
#define PWM_PERIOD_COUNTS           (PWM_TIMER_FREQ_HZ / PWM_FREQUENCY_HZ)
#define PWM_PERIOD                  (PWM_PERIOD_COUNTS - 1U)

/* PWM compare value for duty cycle */
#define PWM_COMPARE                 (PWM_PERIOD_COUNTS * PWM_DUTY_RATIO / 100U)

/* Compile-time validation of timer parameters */
#if PWM_PRESCALER > 65535U
  #error "PWM prescaler value exceeds 16-bit timer range"
#endif

#if PWM_PERIOD > 65535U
  #error "PWM period value exceeds 16-bit timer range"
#endif

#if PWM_COMPARE > PWM_PERIOD
  #error "PWM compare value exceeds period"
#endif

#if PWM_DUTY_RATIO > 100U
  #error "PWM duty ratio cannot exceed 100%"
#endif

/*******************************************************************************
 * TMR14 Register Bit Definitions (CMSIS Style)
 ******************************************************************************/

/* TMR14->swevt (Software Event Register) */
#define TMR_SWEVT_OVFGEN_Pos        (0U)
#define TMR_SWEVT_OVFGEN_Msk        (0x1U << TMR_SWEVT_OVFGEN_Pos)
#define TMR_SWEVT_OVFGEN            TMR_SWEVT_OVFGEN_Msk

/* TMR14->ists (Interrupt Status Register) */
#define TMR_ISTS_OVFIF_Pos          (0U)
#define TMR_ISTS_OVFIF_Msk          (0x1U << TMR_ISTS_OVFIF_Pos)
#define TMR_ISTS_OVFIF              TMR_ISTS_OVFIF_Msk

/* TMR14->ctrl1 (Control Register 1) */
#define TMR_CTRL1_CEN_Pos           (0U)
#define TMR_CTRL1_CEN_Msk           (0x1U << TMR_CTRL1_CEN_Pos)
#define TMR_CTRL1_CEN               TMR_CTRL1_CEN_Msk

/* TMR14->iden (Interrupt/DMA Enable Register) */
#define TMR_IDEN_UIEN_Pos           (0U)
#define TMR_IDEN_UIEN_Msk           (0x1U << TMR_IDEN_UIEN_Pos)
#define TMR_IDEN_UIEN               TMR_IDEN_UIEN_Msk

/* TMR14->cctrl (Capture/Compare Control Register) */
#define TMR_CCTRL_CC1EN_Pos         (0U)
#define TMR_CCTRL_CC1EN_Msk         (0x1U << TMR_CCTRL_CC1EN_Pos)
#define TMR_CCTRL_CC1EN             TMR_CCTRL_CC1EN_Msk

#define TMR_CCTRL_CC1P_Pos          (1U)
#define TMR_CCTRL_CC1P_Msk          (0x1U << TMR_CCTRL_CC1P_Pos)
#define TMR_CCTRL_CC1P              TMR_CCTRL_CC1P_Msk

/* TMR14->cm1 (Capture/Compare Mode Register 1) */
#define TMR_CM1_OC1M_Pos            (4U)
#define TMR_CM1_OC1M_Msk            (0x7U << TMR_CM1_OC1M_Pos)
#define TMR_CM1_OC1M_PWM1           (0x6U << TMR_CM1_OC1M_Pos)

/*******************************************************************************
 * Function Declarations
 ******************************************************************************/

/**
 * @brief Configure TMR14 for PWM generation and event generation
 * 
 * Configuration Summary:
 * - Timer clock source: TIMER_CLOCK_HZ (from crm.h - auto 120MHz/125MHz)
 * - Timer frequency: PWM_TIMER_FREQ_HZ (10kHz)
 * - PWM frequency: PWM_FREQUENCY_HZ (configurable, default 1Hz)
 * - PWM duty cycle: PWM_DUTY_RATIO (configurable, default 10%)
 * - PWM polarity: Active-low
 * - Output pin: PA4 (TMR14_CH1)
 * - Update interrupt: Enabled for WFE events
 * - Register updates: Applied immediately using forced update event
 * 
 * Timer Configuration:
 * - Prescaler: PWM_PRESCALER (calculated from TIMER_CLOCK_HZ)
 * - Period: PWM_PERIOD (calculated)
 * - Compare: PWM_COMPARE (calculated)
 * - Mode: PWM Mode 1
 * - Channel 1: Enabled, active-low polarity
 * 
 * Clock Dependency:
 * - Automatically adjusts for 120MHz (HICK/most HEXT) or 125MHz (25MHz HEXT)
 * - Timer prescaler calculated as: (TIMER_CLOCK_HZ / 10000) - 1
 * - For 120MHz: prescaler = 11999 (10kHz timer base)
 * - For 125MHz: prescaler = 12499 (10kHz timer base)
 * 
 * @note Timer clocks must be enabled before calling this function (done by crm_config)
 * @note GPIO must be configured for TMR14_CH1 alternate function
 */
void timer_config(void);

#endif /* TIMER_H */
