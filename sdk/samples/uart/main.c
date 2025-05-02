#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "scum.h"
#include "uart.h"

static void _uart_cb(uint8_t c) {
    putchar((char)c);
}

int main(void) {
    puts("\nRunning the UART sample application!\n");
    puts("Type anything and press enter to echo it back.\n");

    uart_init(_uart_cb);

    while (1) {
        __WFI();
    }
}
