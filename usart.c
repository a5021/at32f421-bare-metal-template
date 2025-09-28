/**
 * AT32F421 USART Configuration Implementation
 */

#include "usart.h"

/**
 * @brief Configure USART using direct register access with precise baud rate
 */
void usart_config(void) {
    /* Configure USART1 with precise baud rate from centralized clock */
    USART1->baudr = USART_BRR_VALUE;
    USART1->ctrl1 = USART_CTRL1_TEN | USART_CTRL1_REN | USART_CTRL1_UEN;
}

/**
 * @brief Send a single character via USART
 * @param ch Character to send
 */
void usart_putchar(char ch) {
    /* Wait for transmit data register to be empty */
    while (!(USART1->sts & USART_STS_TDBE)) {
        /* Wait */
    }
    
    /* Send character */
    USART1->dt = (uint8_t)ch;
}

/**
 * @brief Send a null-terminated string via USART
 * @param str String to send
 */
void usart_puts(const char* str) {
    while (*str) {
        usart_putchar(*str++);
    }
}

/**
 * @brief Send an unsigned integer as decimal via USART
 * @param value Unsigned integer to send
 */
void usart_put_uint(uint32_t value) {
    char buffer[11];  /* Maximum digits for 32-bit uint + null terminator */
    char* ptr = buffer + sizeof(buffer) - 1;
    
    /* Null terminate */
    *ptr = '\0';
    
    /* Handle zero case */
    if (value == 0) {
        *(--ptr) = '0';
    } else {
        /* Convert digits in reverse */
        while (value > 0) {
            *(--ptr) = '0' + (value % 10);
            value /= 10;
        }
    }
    
    /* Send the string */
    usart_puts(ptr);
}
