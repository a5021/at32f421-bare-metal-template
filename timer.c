/**
 * @file timer.c
 * @brief TMR14 Configuration for PWM and Event Generation
 * 
 * Implementation uses centralized clock configuration from crm.h,
 * automatically handling different system clock frequencies.
 */

#include "timer.h"

/**
 * @brief Configure TMR14 for PWM generation using force update event
 * 
 * This version uses a force update event to ensure new register values
 * are loaded immediately. Clock calculations are automatically adjusted
 * based on system frequency from crm.h.
 */
void timer_config(void) {
    /* Set prescaler - automatically calculated from TIMER_CLOCK_HZ */
    TMR14->div = PWM_PRESCALER;

    /* Set PWM period */
    TMR14->pr = PWM_PERIOD;

    /* Set PWM duty cycle - use c1dt register */
    TMR14->c1dt = PWM_COMPARE;

    /* Configure PWM mode A */
    TMR14->cm1 = TMR_CM1_OC1M_PWM1;

    /* Enable channel 1 output with active-low polarity */
    TMR14->cctrl = TMR_CCTRL_CC1EN | TMR_CCTRL_CC1P;

    /* Enable update interrupt for WFE support */
    TMR14->iden = TMR_IDEN_UIEN;

    /* Force update event to load new register values */
    TMR14->swevt = TMR_SWEVT_OVFGEN;

    /* Clear the stale update flag from the forced event */
    TMR14->ists = 0;

    /* Start timer */
    TMR14->ctrl1 = TMR_CTRL1_CEN;
}
