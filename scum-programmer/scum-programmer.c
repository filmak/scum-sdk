/**
SCuM programmer.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "string.h"
#include "uicr_config.h"

#include <nrf.h>
#include <nrf52840_peripherals.h>

//=========================== defines =========================================

#define UART_BUF_SIZE               1
#define MAX_COMMAND_LEN             64

#define SCUM_MEM_SIZE               65536 //  = 64kB = 64 * 2^10

#define PROGRAMMER_WAIT_4_CMD_ST    0
#define PROGRAMMER_SRAM_LD_ST       1
#define PROGRAMMER_SRAM_LD_DONE     2
#define PROGRAMMER_3WB_BOOT_ST      3

#define CALIBRATION_PORT            0UL
#define PROGRAMMER_EN_PIN           30UL
#define PROGRAMMER_HRST_PIN         31UL
#define PROGRAMMER_CLK_PIN          28UL
#define PROGRAMMER_DATA_PIN         29UL
#define PROGRAMMER_TAP_PIN          3UL

#define CALIBRATION_CLK_PIN         28UL
#define CALIBRATION_PULSE_WIDTH     50   // approximate duty cycle (out of 100)
#define CALIBRATION_PERIOD          100 // period in ms
#define CALIBRATION_FUDGE           308   // # of clock cycles of "fudge"
#define CALIBRATION_NUMBER_OF_PULSES  120 // # of rising edges at 100ms

#define PROGRAMMER_VDDD_HI_PIN      27UL
#define PROGRAMMER_VDDD_LO_PIN      15UL

#define GPIOTE_CALIBRATION_CLOCK    0


static char *UART_TRANSFERSRAM = "transfersram\n";
static char *UART_3WB = "boot3wb\n";
static char *UART_OK = "OK\r\n";
#define UART_OK_MSG_LEN 4U

//=========================== variables =======================================

typedef struct {
    uint8_t        uart_buf_DK_RX[UART_BUF_SIZE];
    uint8_t        uart_buf_DK_TX[UART_BUF_SIZE];

    uint8_t        scum_programmer_state;
    uint8_t        scum_instruction_memory[SCUM_MEM_SIZE];
    uint8_t        uart_RX_command_buf[MAX_COMMAND_LEN];
    uint32_t       uart_RX_command_idx;
    uint32_t       calibration_counter;
} programmer_vars_t;

programmer_vars_t _programmer_vars;


static void setup_clocks(void) {
    NRF_CLOCK->LFCLKSRC                = 0x00000001; // 1==XTAL
    NRF_CLOCK->EVENTS_LFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_LFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);

    NRF_CLOCK->EVENTS_HFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);

}

static void setup_rtc(void) {
    NRF_RTC0->EVTENSET                 = 0x00010000;       // enable compare 0 event routing
    NRF_RTC0->INTENSET                 = 0x00010000;       // enable compare 0 interrupts

    // enable interrupts
    NVIC_SetPriority(RTC0_IRQn, 10);
    NVIC_ClearPendingIRQ(RTC0_IRQn);
    NVIC_EnableIRQ(RTC0_IRQn);
    
    //
    NRF_RTC0->CC[0]                    = (32768>>3);       // 32768>>3 = 125 ms
    NRF_RTC0->TASKS_START              = 0x00000001;       // start RTC0
}

static void setup_uart(void) {
    // configure
    NRF_UARTE0->RXD.PTR                = (uint32_t)_programmer_vars.uart_buf_DK_RX;
    NRF_UARTE0->RXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE0->TXD.PTR                = (uint32_t)_programmer_vars.uart_buf_DK_TX;
    NRF_UARTE0->TXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE0->PSEL.TXD               = 0x00000006; // 0x00000006==P0.6
    NRF_UARTE0->PSEL.RXD               = 0x00000008; // 0x00000008==P0.8
    NRF_UARTE0->CONFIG                 = 0x00000000; // 0x00000000==no flow control, no parity bits, 1 stop bit
    NRF_UARTE0->BAUDRATE               = 0x04000000; // 0x004EA000==19200 baud (actual rate: 19208), 0x04000000==250000 baud (actual rate: 250000)
    NRF_UARTE0->TASKS_STARTRX          = 0x00000001; // 0x00000001==start RX state machine; read received byte from RXD register
    NRF_UARTE0->SHORTS                 = 0x00000020; // short end RX to start RX
    NRF_UARTE0->INTENSET               = 0x00000010;
    NRF_UARTE0->ENABLE                 = 0x00000008; // 0x00000008==enable

    // enable interrupts
    NVIC_SetPriority(UARTE0_UART0_IRQn, 1);
    NVIC_ClearPendingIRQ(UARTE0_UART0_IRQn);
    NVIC_EnableIRQ(UARTE0_UART0_IRQn);
}

static void bootloader_init(void) {
    NRF_P0->PIN_CNF[PROGRAMMER_DATA_PIN]    = 0x00000003; // 0x03 configures pins as an output pin and disconnects the input buffer
    NRF_P0->PIN_CNF[PROGRAMMER_CLK_PIN]     = 0x00000003;
    NRF_P0->PIN_CNF[PROGRAMMER_HRST_PIN]    = 0x00000000; // 0x00 configures the pin as an input, input buffer disconnected, pull up/down disabled (no pull)
    NRF_P0->PIN_CNF[PROGRAMMER_EN_PIN]      = 0x00000003;
    NRF_P0->PIN_CNF[PROGRAMMER_TAP_PIN]     = 0x00000000; // default to hi-Z
    NRF_P0->PIN_CNF[PROGRAMMER_VDDD_HI_PIN] = 0x00000303;
    NRF_P1->PIN_CNF[PROGRAMMER_VDDD_LO_PIN] = 0x00000303;

    NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_VDDD_HI_PIN;
    NRF_P1->OUTCLR = (0x00000001) << PROGRAMMER_VDDD_LO_PIN;
}

static void calibration_gpiote_init(void) {
    NRF_GPIOTE->CONFIG[GPIOTE_CALIBRATION_CLOCK] =  ((3UL) << (0UL))    |                 // enable GPIOTE task
                                                    (CALIBRATION_CLK_PIN << (8UL))    |   // set pin #
                                                    (CALIBRATION_PORT << (13UL))      |   // set port #
                                                    ((3UL) << (16UL))                 |   // 3UL -> toggle pin on each event
                                                    ((1UL) << (20UL))                 ;   // 0UL -> initialize pin to LOW
}

static void calibration_gpiote_disable(void) {
    NRF_GPIOTE->CONFIG[GPIOTE_CALIBRATION_CLOCK] =  ((0UL) << (0UL))                    |   // disable GPIOTE task
                                                    ((CALIBRATION_CLK_PIN) << (8UL))    |   // on pin #
                                                    ((CALIBRATION_PORT) << (13UL))      |   // port #
                                                    ((0UL) << (16UL))                   |   // do not toggle
                                                    ((0UL) << (20UL))                   ;   // do not initialize
}

static void calibration_timer2_init(void) {
    NRF_TIMER2->BITMODE = (3UL); // set to 32-bit timer bit width
    NRF_TIMER2->PRESCALER = (0UL); // set prescaler to zero - default is pre-scale by 16

    NRF_TIMER2->CC[1]   = 40;
    NRF_TIMER2->CC[2]   = CALIBRATION_PERIOD * 16000 - CALIBRATION_FUDGE; // artificially remove the N clk cycle delay in the PPI

    NRF_TIMER2->SHORTS  =  ((0UL) << (2UL)); // short compare[2] event and clear
}

static void calibration_PPI_init(void) {
    // connect endpoints
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[2];
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[GPIOTE_CALIBRATION_CLOCK];
    NRF_PPI->FORK[0].TEP = (uint32_t)&NRF_TIMER2->TASKS_START;

    NRF_PPI->CH[1].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[1];
    NRF_PPI->CH[1].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[GPIOTE_CALIBRATION_CLOCK];

    // enable channels
    NRF_PPI->CHENSET = ((1UL) << (0UL)) | ((1UL) << (1UL));
}

static void calibration_init(void) {

    calibration_gpiote_init();
    calibration_timer2_init();
    calibration_PPI_init();

    NVIC_SetPriority(TIMER2_IRQn, 10);
    NVIC_EnableIRQ(TIMER2_IRQn);
    NRF_TIMER2->INTENCLR = (1UL)<<18;
    NRF_TIMER2->INTENSET = (1UL)<<18;

}

static void busy_wait_us(uint32_t us) {
    uint32_t delay = 3 * us;
    while (delay--) {}
}

static void busy_wait_ms(uint32_t ms) {
    busy_wait_us(ms * 1000);
}

static void print_3wb_done_msg(void) {
    for (uint8_t i = 0; i < UART_OK_MSG_LEN; i++) {
        _programmer_vars.uart_buf_DK_TX[0] = UART_OK[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_ms(1); //TODO: this should not be necessary... but it is?
    }
}

static void print_sram_started_msg(void) {
    for (uint8_t i = 0; i < UART_OK_MSG_LEN; i++) {
        _programmer_vars.uart_buf_DK_TX[0] = UART_OK[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_ms(1); //TODO: this should not be necessary... but it is?
    }
}

static void print_sram_done_msg(void) {
    for (uint8_t i = 0; i < UART_OK_MSG_LEN; i++) {
        _programmer_vars.uart_buf_DK_TX[0] = UART_OK[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_ms(1); //TODO: this should not be necessary... but it is?
    }
}


int main(void) {
    setup_clocks();
    setup_rtc();

    // initialize bootloader state
    bootloader_init();
    busy_wait_ms(1);

    _programmer_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;

    while(1) {
        setup_uart();

        // wait for event
        __SEV(); // set event
        __WFE(); // wait for event
        __WFE(); // wait for event
    }
}

void RTC0_IRQHandler(void) {
    if (NRF_RTC0->EVENTS_COMPARE[0]) {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;
        NRF_RTC0->TASKS_CLEAR = 1;
     }
}

void TIMER2_IRQHandler(void) {

    // handle compare[1]
    if (NRF_TIMER2->EVENTS_COMPARE[1]) {
        NRF_TIMER2->EVENTS_COMPARE[1] = 0;
        NRF_TIMER2->CC[1] = 0;
    }

    // handle compare[2]
    if (NRF_TIMER2->EVENTS_COMPARE[2]) {
        // no need to clear - it is done w/ a short
        NRF_TIMER2->TASKS_CLEAR = 1;
        NRF_TIMER2->EVENTS_COMPARE[2] = 0;
        _programmer_vars.calibration_counter++;

        // re-enable CC[1]
        NRF_TIMER2->CC[1] = 300;

        if (_programmer_vars.calibration_counter>(10)) {
            NRF_P0->PIN_CNF[PROGRAMMER_TAP_PIN] = 0x00000000; // turn the tap off after 5 100ms cycles!
        }

        if (_programmer_vars.calibration_counter>(CALIBRATION_NUMBER_OF_PULSES)) { 
            _programmer_vars.calibration_counter = 0;

            NRF_TIMER2->TASKS_STOP = 1; // stop the count!
            calibration_gpiote_disable(); // disable gpiote on data pin so a second program can occur :)
            NRF_GPIOTE->CONFIG[0] = 0;
            NRF_P0->PIN_CNF[CALIBRATION_CLK_PIN] = 0x00000003;
            _programmer_vars.uart_RX_command_idx = 0;
        }
    }
}

void UARTE0_UART0_IRQHandler(void) {

    uint8_t uart_rx_byte;

    if (NRF_UARTE0->EVENTS_ENDRX) {
        // byte received from DK

        // clear
        NRF_UARTE0->EVENTS_ENDRX = 0;

        if(_programmer_vars.scum_programmer_state == PROGRAMMER_WAIT_4_CMD_ST) {
            uart_rx_byte = _programmer_vars.uart_buf_DK_RX[0];
            _programmer_vars.uart_RX_command_buf[_programmer_vars.uart_RX_command_idx++] = uart_rx_byte;

            if((uart_rx_byte=='\n')||(uart_rx_byte=='\r')) { // \r for debugging w/ putty, \n for the python scripts
                _programmer_vars.uart_RX_command_idx = 0; // reset index to receive the next command
                if(memcmp(_programmer_vars.uart_RX_command_buf,UART_TRANSFERSRAM,strlen(UART_TRANSFERSRAM))==0) { // enter transfer SRAM state
                    _programmer_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;
                    _programmer_vars.uart_RX_command_idx = 0;
                    print_sram_started_msg();
                }

                else if (memcmp(_programmer_vars.uart_RX_command_buf, UART_3WB, strlen(UART_3WB))==0) { // enter 3WB state
                    _programmer_vars.scum_programmer_state = PROGRAMMER_3WB_BOOT_ST;
                }
                // else - erroneous command, clear the buffer and reset to default state
                else {
                    memset(_programmer_vars.uart_RX_command_buf,0,sizeof(_programmer_vars.uart_RX_command_buf));
                    _programmer_vars.uart_RX_command_idx = 0;
                    _programmer_vars.scum_programmer_state = PROGRAMMER_WAIT_4_CMD_ST;
                }
            }
            else if (_programmer_vars.uart_RX_command_idx > MAX_COMMAND_LEN) { // max length exceeded w/out return character, reset buffer
                memset(_programmer_vars.uart_RX_command_buf,0,sizeof(_programmer_vars.uart_RX_command_buf));
                _programmer_vars.uart_RX_command_idx = 0;
                _programmer_vars.scum_programmer_state = PROGRAMMER_WAIT_4_CMD_ST;
            }
        }
        else if (_programmer_vars.scum_programmer_state == PROGRAMMER_SRAM_LD_ST) {
            uart_rx_byte = _programmer_vars.uart_buf_DK_RX[0];
            _programmer_vars.scum_instruction_memory[_programmer_vars.uart_RX_command_idx++] = uart_rx_byte;
            if(_programmer_vars.uart_RX_command_idx == SCUM_MEM_SIZE) { // finished w/ the memory
                // after loading memory - reset state, index, and command buffer
                print_sram_done_msg();
                _programmer_vars.scum_programmer_state = PROGRAMMER_3WB_BOOT_ST;
                memset(_programmer_vars.uart_RX_command_buf,0,sizeof(_programmer_vars.uart_RX_command_buf));
            }
        }
        
        if (_programmer_vars.scum_programmer_state == PROGRAMMER_3WB_BOOT_ST) {
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_CLK_PIN;
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_DATA_PIN;
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_EN_PIN;

            // execute hard reset (debug for now)
            NRF_P0->PIN_CNF[PROGRAMMER_HRST_PIN] = 0x00000003; // configure as output, set low
            busy_wait_ms(14);
            NRF_P0->PIN_CNF[PROGRAMMER_HRST_PIN] = 0x00000000; // return to input
            busy_wait_ms(14);

            for (uint32_t i=1; i<SCUM_MEM_SIZE+1; i++) {
                for (uint8_t j=0; j<8; j++) {

                    if (((_programmer_vars.scum_instruction_memory[i-1]>>j)&(0x01))==0x01) {
                        NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_DATA_PIN;
                    }
                    else if (((_programmer_vars.scum_instruction_memory[i-1]>>j)&(0x01))==0x00) {
                        NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_DATA_PIN;
                    }
                    busy_wait_us(1);
                    if ((i%4 == 0) && (j==7)) {
                        NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    else {
                        NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    // toggle the clock
                    busy_wait_us(1);
                    NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_us(1);
                    NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_us(1);
                }
            }

            // after bootloading - return to "load" state (currently debugging)
            print_3wb_done_msg();
            _programmer_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;
            _programmer_vars.uart_RX_command_idx = 0;
            memset(_programmer_vars.uart_RX_command_buf,0,sizeof(_programmer_vars.uart_RX_command_buf));

            // experimental - after bootloading, do a tap
            NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_TAP_PIN; // first set pin high - NEVER CLEAR!!! scum will hate it if you do
            NRF_P0->PIN_CNF[PROGRAMMER_TAP_PIN] = 0x00000003; // then enable output

            // optional - start the 100ms calibration clock
            // initialize 100ms clock pin
            calibration_init();
            _programmer_vars.calibration_counter = 0;
            NRF_TIMER2->TASKS_START = 1UL;


        }
    }
}
