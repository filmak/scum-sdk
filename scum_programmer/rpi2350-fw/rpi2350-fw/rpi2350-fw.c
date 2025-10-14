/**
SCuM programmer.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "hardware/structs/dma.h"
#include "hardware/regs/dma.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/regs/timer.h"
#include "hardware/structs/timer.h"
#include "hdlc.h"

//=========================== defines =========================================

#define UART_BUF_SIZE    (32U)
#define COMMAND_BUF_SIZE (2048U)
#define CHUNK_SIZE       (1024U)
#define SCUM_MEM_SIZE    (1 << 16)  // 64KiB

#define PROGRAMMER_EN_PIN   4
#define PROGRAMMER_HRST_PIN 5
#define PROGRAMMER_CLK_PIN  2
#define PROGRAMMER_DATA_PIN 3

#define CALIBRATION_CLK_PIN          28UL
#define CALIBRATION_PULSE_WIDTH      50   // approximate duty cycle (out of 100)
#define CALIBRATION_PERIOD           100000  // period in us
#define CALIBRATION_FUDGE            308  // # of clock cycles of "fudge"
#define CALIBRATION_NUMBER_OF_PULSES 10   // # of rising edges at 100ms

#define PROGRAMMER_VDDD_HI_PIN 27UL
#define PROGRAMMER_VDDD_LO_PIN 15UL

#define GPIOTE_CALIBRATION_CLOCK 0

// specific to RPI pico2
#define UART_ID uart1
#define BAUD_RATE 115200
// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

//=========================== variables =======================================

typedef enum {
    COMMAND_START = 0x01,
    COMMAND_CHUNK = 0x02,
    COMMAND_BOOT = 0x03,
    COMMAND_CALIBRATE = 0x04,
} command_type_t;

typedef struct __attribute__((packed)) {
    command_type_t type;
    uint8_t buffer[COMMAND_BUF_SIZE];
} uart_command_t;

typedef struct {
    bool uart_byte_received;
    uint8_t uart_rx_byte;
    uint8_t uart_tx_buf[UART_BUF_SIZE];
    uart_command_t uart_command;

    uint32_t chunk_idx;

    bool calibration_done;
    uint32_t calibration_counter;
} programmer_vars_t;

static programmer_vars_t _programmer_vars = { 0 };

static const char *UART_ACK = "ACK\n";

void timer_irq(void) {
    // handle compare[1]
}

static void setup_programmer(void) {
    gpio_init(PROGRAMMER_CLK_PIN);
    gpio_set_dir(PROGRAMMER_CLK_PIN, GPIO_OUT);
    gpio_init(PROGRAMMER_DATA_PIN);
    gpio_set_dir(PROGRAMMER_DATA_PIN, GPIO_OUT);
    gpio_init(PROGRAMMER_EN_PIN);
    gpio_set_dir(PROGRAMMER_EN_PIN, GPIO_OUT);
  
    // The hard reset pin is set to high-Z.
    gpio_init(PROGRAMMER_HRST_PIN);
    gpio_set_dir(PROGRAMMER_HRST_PIN, GPIO_IN);
  
    // Disable all pull-up and pull-down resistors.
    gpio_disable_pulls(PROGRAMMER_CLK_PIN);
    gpio_disable_pulls(PROGRAMMER_DATA_PIN);
    gpio_disable_pulls(PROGRAMMER_EN_PIN);
    gpio_disable_pulls(PROGRAMMER_HRST_PIN);
  
    // Decrease the drive strengths of the GPIOs.
    gpio_set_drive_strength(PROGRAMMER_CLK_PIN, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(PROGRAMMER_DATA_PIN, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(PROGRAMMER_EN_PIN, GPIO_DRIVE_STRENGTH_2MA);
    gpio_set_drive_strength(PROGRAMMER_HRST_PIN, GPIO_DRIVE_STRENGTH_2MA);
}

static void setup_timer2(void) {
    timer_hw->alarm[2] = timer_hw->timerawl + CALIBRATION_PERIOD;
    hw_set_bits(&timer_hw->inte, 1u << 2);
    irq_set_exclusive_handler(TIMER0_IRQ_0, timer_irq);
    irq_set_enabled(TIMER0_IRQ_0, true);
}

static void setup_uart(void) {
    // RPi pico2 specific: all comms are done over USB, not UART
    // Initialize USB.
    stdio_usb_init();

    // Limit input and output to USB only.
    stdio_filter_driver(&stdio_usb);
}

static void uart_write(const uint8_t *buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        putchar_raw(buffer[i]);  // send one byte directly over USB CDC
    }
}

static void poll_usb_rx(void) {
    int c = getchar_timeout_us(1000);  // non-blocking read from USB CDC
    if (c != PICO_ERROR_TIMEOUT) {
        _programmer_vars.uart_rx_byte = (uint8_t)c;
        _programmer_vars.uart_byte_received = true;
    }
}

static void run_calibration(void) {
    setup_timer2();

    //NVIC_EnableIRQ(TIMER2_IRQn);
    //NRF_TIMER2->INTENCLR = TIMER_INTENCLR_COMPARE2_Enabled << TIMER_INTENCLR_COMPARE2_Pos;
    //NRF_TIMER2->INTENSET = TIMER_INTENSET_COMPARE2_Enabled << TIMER_INTENSET_COMPARE2_Pos;
    //NRF_TIMER2->TASKS_START = 1;

    while (!_programmer_vars.calibration_done) {
        asm volatile("" :::);
    }
    _programmer_vars.calibration_counter = 0;
    _programmer_vars.calibration_done = false;
}


static void bitband_byte(uint8_t byte, bool latch) {
    for (uint8_t j = 0; j < 8; j++) {
        if ((byte >> j) & 0x01) {
            gpio_put(PROGRAMMER_DATA_PIN, true);
        } else if (!((byte >> j) & 0x01)) {
            gpio_put(PROGRAMMER_DATA_PIN, false);
        }
        if (latch && (j == 7)) {
            gpio_put(PROGRAMMER_EN_PIN, true);
        } else {
            gpio_put(PROGRAMMER_EN_PIN, false);
        }
        // toggle the clock
        gpio_put(PROGRAMMER_CLK_PIN, true);
        gpio_put(PROGRAMMER_CLK_PIN, false);
    }
}

static void _process_command(void) {
    hdlc_decode((uint8_t *)&_programmer_vars.uart_command);
    switch (_programmer_vars.uart_command.type) {
        case COMMAND_START:
        {
            //puts("START");
            _programmer_vars.chunk_idx = 0;

            //NRF_P0->OUTCLR = 1 << PROGRAMMER_CLK_PIN;
            gpio_put(PROGRAMMER_CLK_PIN, false);
            //NRF_P0->OUTCLR = 1 << PROGRAMMER_DATA_PIN;
            gpio_put(PROGRAMMER_DATA_PIN, false);
            //NRF_P0->OUTCLR = 1 << PROGRAMMER_EN_PIN;
            gpio_put(PROGRAMMER_EN_PIN, false);
            // execute hard reset (debug for now)
            gpio_put(PROGRAMMER_HRST_PIN, false);
            gpio_set_dir(PROGRAMMER_HRST_PIN, GPIO_OUT); // configure as output
            gpio_put(PROGRAMMER_HRST_PIN, false); // set value to zero (HRESET is active low)
            busy_wait_ms(14);
            gpio_set_dir(PROGRAMMER_HRST_PIN, GPIO_IN); // return to input
            busy_wait_ms(14);
            break;
        }
        case COMMAND_CHUNK:
        {
            for (uint32_t idx = 1; idx < CHUNK_SIZE + 1; idx++) {
                bitband_byte(_programmer_vars.uart_command.buffer[idx - 1], (idx % 4 == 0));
            }
            _programmer_vars.chunk_idx++;
            break;
        }
        case COMMAND_BOOT:
        {
            uint32_t received_bytes = _programmer_vars.chunk_idx * CHUNK_SIZE;
            uint32_t remaining_bytes = SCUM_MEM_SIZE - received_bytes;
            for (uint32_t idx = 1; idx < remaining_bytes + 1; idx++) {
                bitband_byte(0x00, (idx % 4 == 0));
            }
            break;
        }
        case COMMAND_CALIBRATE:
            puts("CALIBRATE");
            run_calibration();
            break;
        default:
            break;
    }

    uart_write((uint8_t *)UART_ACK, strlen(UART_ACK));
}


int main()
{
    setup_uart();

    while (true) {
        poll_usb_rx();
        if (_programmer_vars.uart_byte_received) {
            hdlc_state_t state = hdlc_rx_byte(_programmer_vars.uart_rx_byte);
            if (state == HDLC_STATE_READY) {
                _process_command();
            }
            _programmer_vars.uart_byte_received = false;
        }
    }
}