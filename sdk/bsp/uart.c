#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scum.h"
#include "uart.h"

// Flow control constants.
// XOFF, XON, and XONXOFF_ESCAPE are transmitted as two bytes: {XONXOFF_ESCAPE,
// data ^ XONXOFF_MASK}.
enum {
    XOFF = 0x13,
    XON = 0x11,
    XONXOFF_ESCAPE = 0x12,
    XONXOFF_MASK = 0x10,
};

// UART TX callback function.
static uart_tx_callback_t g_uart_tx_callback;

// UART RX callback function.
static uart_rx_callback_t g_uart_rx_callback;

// UART XON/XOFF escaping. If true, the current data character is being escaped
// and has to be transmitted after the escape character.
static bool g_uart_xon_xoff_escaping = false;

// UART XON/XOFF escaped character.
static char g_uart_xon_xoff_escaped_char = 0;

// UART TX interrupt service routine.
void uart_tx_isr(void) {
    if (g_uart_xon_xoff_escaping) {
        SCUM_UART->DATA = g_uart_xon_xoff_escaped_char ^ XONXOFF_MASK;
        g_uart_xon_xoff_escaping = false;
    }

    if (g_uart_tx_callback != NULL) {
        g_uart_tx_callback();
    }
}

// UART RX interrupt service routine.
void UART_Handler(void) {
    if (g_uart_rx_callback != NULL) {
        g_uart_rx_callback(uart_read());
    }
}

void uart_set_tx_callback(const uart_tx_callback_t callback) {
    g_uart_tx_callback = callback;
}

void uart_set_rx_callback(const uart_rx_callback_t callback) {
    g_uart_rx_callback = callback;
}

void uart_enable_interrupt(void) {
    NVIC_EnableIRQ(UART_IRQn);
    printf("UART interrupt enabled: 0x%08lx.\n", *NVIC->ISER);
}

void uart_disable_interrupt(void) {
    NVIC_DisableIRQ(UART_IRQn);
    printf("UART interrupt disabled: 0x%08lx.\n", *NVIC->ICER);
}

void uart_set_cts(const bool state) {
    if (state) {
        SCUM_UART->DATA = XON;
    } else {
        SCUM_UART->DATA = XOFF;
    }
}

void uart_write(const char data) {
    if (data == XOFF || data == XON || data == XONXOFF_MASK) {
        g_uart_xon_xoff_escaping = true;
        g_uart_xon_xoff_escaped_char = data;
        SCUM_UART->DATA = XONXOFF_ESCAPE;
    } else {
        SCUM_UART->DATA = data;
    }

    // There is no TX done interrupt, so call the interrupt handler directly.
    uart_tx_isr();
}

char uart_read(void) { return SCUM_UART->DATA; }
