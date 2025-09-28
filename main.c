/**
 * @file   main.c
 * @brief  AT32F421 PWM Demo with an Event-Driven Main Loop
 *
 * @note   This application demonstrates a low-power, event-driven architecture
 *         on an ARM Cortex-M microcontroller.
 *
 * Key Architectural Features:
 *   - System Initialization: A single `system_init()` orchestrates all hardware setup.
 *   - Event-Driven Loop: The main `while(1)` loop is driven by timer events.
 *   - Low-Power Sleep: `__WFE()` (Wait For Event) is used to put the core to sleep.
 *   - Interrupt-less Wake-up: `SEVONPEND` allows timer flags to wake the core
 *     without needing a full interrupt service routine (ISR).
 *   - Race-Condition Handling: The code is structured to handle race conditions
 *     where events occur during initialization, using the `__SEV()` + `__WFE()` idiom.
 */

#include "at32f421.h"
#include "crm.h"
#include "gpio.h" 
#include "timer.h"
#include "usart.h"

#define DEBUG_ENABLED 1

/* The IRQn for TMR14 is needed for clearing the NVIC pending bit.
 * This value should be verified in the device's official startup file. */
#ifndef TMR14_GLOBAL_IRQn
  #define TMR14_GLOBAL_IRQn  19
#endif

/* Private function prototypes */
static void system_init(void);
static void print_system_info(void);
static void print_runtime_stats(void);

/* Global counters for runtime statistics. */
static uint32_t timer_overflow_count = 0;
static uint32_t wfe_wake_count = 0;

/**
 * @brief  Main application entry point.
 */
int main(void) {

    system_init();
    print_system_info();
    
    while (1) {
        /*
         * 1. Clear the NVIC pending bit for the TMR14 interrupt. This is required
         *    to allow the next interrupt to trigger a wake-up event, as SEVONPEND
         *    only generates an event on a 0-to-1 transition of the pending bit.
         */
        NVIC_ClearPendingIRQ(TMR14_GLOBAL_IRQn);
        
        /*
         * 2. Execute Wait For Event. The ARM core's internal event latch ensures
         *    that no events are missed if they occur between the clear above
         *    and this instruction.
         */
        __WFE();
        
        wfe_wake_count++;
        
        /* 3. Check the peripheral's flag to confirm the source of the wake-up. */
        if (TMR14->ists & TMR_ISTS_OVFIF) {
            TMR14->ists &= ~TMR_ISTS_OVFIF;
            timer_overflow_count++;
            print_runtime_stats();
        }
    }

    return 0;
}

/**
 * @brief  Initializes all system hardware and clears any spurious startup events.
 */
static void system_init(void) {
    crm_config();
    gpio_config();
    timer_config();
    usart_config();
    
    /* Enable SEVONPEND to allow pending interrupts to generate __WFE() events. */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    /*
     * Clear any spurious, pending events from the initialization phase.
     * The __SEV() instruction unconditionally sets the event latch, ensuring
     * that the subsequent __WFE() consumes the event and continues immediately
     * without sleeping. This is the architecturally-defined method for
     * clearing the event state.
     */
    __SEV();
    __WFE();
}

/**
 * @brief  Prints a startup banner with key system parameters.
 */
static void print_system_info(void) {
#if DEBUG_ENABLED
    usart_puts("\r\nAT32F421 PWM Demo with WFE\r\n");
    usart_puts("---------------------------\r\n");
    usart_puts("SYSCLK: ");
    usart_put_uint(SYSTEM_CLOCK_HZ / 1000000);
    usart_puts("MHz, PWM Freq: ");
    usart_put_uint(PWM_FREQUENCY_HZ);
    usart_puts("Hz, Duty: ");
    usart_put_uint(PWM_DUTY_RATIO);
    usart_puts("%\r\n");
    usart_puts("Power Mode: SEVONPEND + WFE Enabled\r\n\r\n");
#endif
}

/**
 * @brief  Prints runtime statistics periodically.
 */
static void print_runtime_stats(void) {
#if DEBUG_ENABLED
    static uint32_t last_print_time = 0;
    uint32_t current_time = timer_overflow_count;
    
    if ((current_time - last_print_time) >= 5) {
        usart_puts("TMR Events: ");
        usart_put_uint(timer_overflow_count);
        usart_puts(", WFE Wakes: ");
        usart_put_uint(wfe_wake_count);
        
        if (wfe_wake_count > 0) {
            uint32_t efficiency = (timer_overflow_count * 100) / wfe_wake_count;
            usart_puts(", Efficiency: ");
            usart_put_uint(efficiency);
            usart_puts("%");
        }
        usart_puts("\r\n");
        last_print_time = current_time;
    }
#endif
}

/**
 * @brief  Hard Fault Handler.
 */
void HardFault_Handler(void) {
#if DEBUG_ENABLED
    usart_puts("\r\n*** HARD FAULT! ***\r\nSystem Halted.\r\n");
#endif
    while (1) {
        __NOP();
    }
}
