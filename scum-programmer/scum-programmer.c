/**
SCuM programmer.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "string.h"
#include "uicr_config.h"
#include "nrf52840.h"

//=========================== defines =========================================

const uint8_t APP_VERSION[]         = {0x00,0x01};

#define UART_BUF_SIZE               1
#define NUM_LEDS                    4
#define MAX_COMMAND_LEN             64

#define SCUM_MEM_SIZE               65536 //  = 64kB = 64 * 2^10

#define PROGRAMMER_WAIT_4_CMD_ST    0
#define PROGRAMMER_SRAM_LD_ST       1
#define PROGRAMMER_SRAM_LD_DONE     2
#define PROGRAMMER_3WB_BOOT_ST      3
#define PROGRAMMER_OPT_BOOT_ST      4
#define PROGRAMMER_3WB_BOOT_DONE    5
#define PROGRAMMER_OPT_BOOT_DONE    6
#define PROGRAMMER_DBG_ST           38
#define PROGRAMMER_ERR_ST           0xFF

#define PROGRAMMER_PORT             0
#define PROGRAMMER_EN_PIN           30
#define PROGRAMMER_HRST_PIN         31
#define PROGRAMMER_CLK_PIN          28
#define PROGRAMMER_DATA_PIN         29

// https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrf52840_dk%2FUG%2Fdk%2Fhw_buttons_leds.html
// Button 1 P0.11
// Button 2 P0.12
// Button 3 P0.24
// Button 4 P0.25
// LED 1 P0.13
// LED 2 P0.14
// LED 3 P0.15
// LED 4 P0.16

static uint8_t UART_TRANSFERSRAM[] = "transfersram\n";
static uint8_t UART_3WB[] = "boot3wb\n"; // TODO: change these to \n once I'm done debugging with putty

static uint8_t SRAM_LOAD_START_MSG[] = "SRAM load ready\r\n";
#define SRAM_LOAD_START_MSG_LEN 17
static uint8_t SRAM_LOAD_DONE_MSG[] = "SRAM load complete\r\n";
#define SRAM_LOAD_DONE_MSG_LEN 20
static uint8_t PROG_3WB_DONE_MSG[] = "3WB bootload complete\r\n";
#define PROG_3WB_DONE_MSG_LEN 23
static uint8_t PROG_OPT_DONE_MSG[] = "Optical bootload complete\r\n";

//=========================== prototypes ======================================

void lfxtal_start(void);
void hfclock_start(void);
void led_enable(void);
void led_advance(void);
void uarts_init(void);
void bootloader_init(void);
void busy_wait_1us(void);
void busy_wait_200us(void);
void busy_wait_1ms(void);
void print_3wb_done_msg(void);
void print_sram_started_msg(void);
void print_sram_done_msg(void);

//=========================== variables =======================================

typedef struct {
    uint32_t       led_counter;
    uint8_t        uart_buf_DK_RX[UART_BUF_SIZE];
    uint8_t        uart_buf_DK_TX[UART_BUF_SIZE];
    uint8_t        uart_buf_SCuM_RX[UART_BUF_SIZE];
    uint8_t        uart_buf_SCuM_TX[UART_BUF_SIZE];

    uint8_t        scum_programmer_state;
    uint8_t        scum_instruction_memory[SCUM_MEM_SIZE];
    uint8_t        uart_RX_command_buf[MAX_COMMAND_LEN];
    uint32_t       uart_RX_command_idx;
} app_vars_t;
app_vars_t app_vars;

typedef struct {
    uint32_t       num_task_loops;
    uint32_t       num_ISR_RTC0_IRQHandler;
    uint32_t       num_ISR_RTC0_IRQHandler_COMPARE0;
    uint32_t       num_ISR_UARTE0_UART0_IRQHandler;
    uint32_t       num_ISR_UARTE0_UART0_IRQHandler_ENDRX;
    uint32_t       num_ISR_UARTE1_IRQHandler;
    uint32_t       num_ISR_UARTE1_IRQHandler_ENDRX;
} app_dbg_t;
app_dbg_t app_dbg;

//=========================== main ============================================



// 

int main(void) {

    // initialize bootloader state
    app_vars.scum_programmer_state = PROGRAMMER_WAIT_4_CMD_ST;
    bootloader_init();

    // main loop
    while(1) {
        
        // bsp
        lfxtal_start();
        hfclock_start();
        led_enable();
        uarts_init();

        // wait for event
        __SEV(); // set event
        __WFE(); // wait for event
        __WFE(); // wait for event

        // debug
        app_dbg.num_task_loops++;
    }
}

//=========================== bsp =============================================

//=== lfxtal

void lfxtal_start(void) {
    
    // start 32kHz XTAL
    NRF_CLOCK->LFCLKSRC                = 0x00000001; // 1==XTAL
    NRF_CLOCK->EVENTS_LFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_LFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);

}

//=== hfclock

void hfclock_start(void) {
    
    NRF_CLOCK->EVENTS_HFCLKSTARTED     = 0;
    NRF_CLOCK->TASKS_HFCLKSTART        = 0x00000001;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

//=== led

void bootloader_init(void) {
    if (PROGRAMMER_PORT == 0) {
        NRF_P0->PIN_CNF[PROGRAMMER_DATA_PIN]    = 0x00000003;
        NRF_P0->PIN_CNF[PROGRAMMER_CLK_PIN]     = 0x00000003;
        NRF_P0->PIN_CNF[PROGRAMMER_HRST_PIN]    = 0x00000003;
        NRF_P0->PIN_CNF[PROGRAMMER_EN_PIN]      = 0x00000003;
    }
    else if (PROGRAMMER_PORT == 1) {
        NRF_P1->PIN_CNF[PROGRAMMER_DATA_PIN]    = 0x00000003;
        NRF_P1->PIN_CNF[PROGRAMMER_CLK_PIN]     = 0x00000003;
        NRF_P1->PIN_CNF[PROGRAMMER_HRST_PIN]    = 0x00000003;
        NRF_P1->PIN_CNF[PROGRAMMER_EN_PIN]      = 0x00000003;
    }
}

void busy_wait_1us(void) {
    int k=0;
    for (int i=0; i<3; i++) {
        k++;
    }
}
void busy_wait_200us(void) {
    int k=0;
    for (int i=0; i<600; i++) {
        k++;
    }
}
void busy_wait_1ms(void) {
    int k=0;
    for (int i=0; i<3000; i++) {
        k++;
    }
}

void led_enable(void) {
    // do after LF XTAL started

    // enable all LEDs
    NRF_P0->PIN_CNF[13]                = 0x00000003;            // LED 1
    NRF_P0->PIN_CNF[14]                = 0x00000003;            // LED 2
    NRF_P0->PIN_CNF[15]                = 0x00000003;            // LED 3
    NRF_P0->PIN_CNF[16]                = 0x00000003;            // LED 4
    
    // configure RTC0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // xxxx xxxx xxxx FEDC xxxx xxxx xxxx xxBA (C=compare 0)
    // 0000 0000 0000 0001 0000 0000 0000 0000 
    //    0    0    0    1    0    0    0    0 0x00010000
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

void led_advance(void) {
    
    // bump
    app_vars.led_counter               = (app_vars.led_counter+1)%NUM_LEDS;

    // apply
    NRF_P0->OUTSET                     = (0x00000001 << 13);
    NRF_P0->OUTSET                     = (0x00000001 << 14);
    NRF_P0->OUTSET                     = (0x00000001 << 15);
    NRF_P0->OUTSET                     = (0x00000001 << 16);
    switch (app_vars.led_counter) {
        case 0: NRF_P0->OUTCLR         = (0x00000001 << 13); break; // LED 1
        case 1: NRF_P0->OUTCLR         = (0x00000001 << 14); break; // LED 2
        case 2: NRF_P0->OUTCLR         = (0x00000001 << 16); break; // LED 4
        case 3: NRF_P0->OUTCLR         = (0x00000001 << 15); break; // LED 3
    }
}

void uarts_init(void) {
    // do after HFCLOCK started

    //=== UART0 to DK
    
    // https://infocenter.nordicsemi.com/topic/ug_nrf52840_dk/UG/dk/vir_com_port.html
    // P0.05	RTS
    // P0.06	TXD <===
    // P0.07	CTS
    // P0.08	RXD <===
    
    // configure
    NRF_UARTE0->RXD.PTR                = (uint32_t)app_vars.uart_buf_DK_RX;
    NRF_UARTE0->RXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE0->TXD.PTR                = (uint32_t)app_vars.uart_buf_DK_TX;
    NRF_UARTE0->TXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE0->PSEL.TXD               = 0x00000006; // 0x00000006==P0.6
    NRF_UARTE0->PSEL.RXD               = 0x00000008; // 0x00000008==P0.8
    NRF_UARTE0->CONFIG                 = 0x00000000; // 0x00000000==no flow control, no parity bits, 1 stop bit
    NRF_UARTE0->BAUDRATE               = 0x04000000; // 0x004EA000==19200 baud (actual rate: 19208)
    NRF_UARTE0->TASKS_STARTRX          = 0x00000001; // 0x00000001==start RX state machine; read received byte from RXD register
    NRF_UARTE0->SHORTS                 = 0x00000020; // short end RX to start RX
    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: CTS
    // .... .... .... .... .... .... .... ..B. B: NCTS
    // .... .... .... .... .... .... .... .C.. C: RXDRDY
    // .... .... .... .... .... .... ...D .... D: ENDRX
    // .... .... .... .... .... .... E... .... E: TXDRDY
    // .... .... .... .... .... ...F .... .... F: ENDTX
    // .... .... .... .... .... ..G. .... .... G: ERROR
    // .... .... .... ..H. .... .... .... .... H: RXTO
    // .... .... .... I... .... .... .... .... I: RXSTARTED
    // .... .... ...J .... .... .... .... .... J: TXSTARTED
    // .... .... .L.. .... .... .... .... .... L: TXSTOPPED
    // xxxx xxxx x0x0 0x0x xxxx xx00 0xx1 x000 
    //    0    0    0    0    0    0    1    0 0x00000010
    NRF_UARTE0->INTENSET               = 0x00000010;
    NRF_UARTE0->ENABLE                 = 0x00000008; // 0x00000008==enable

    // enable interrupts
    NVIC_SetPriority(UARTE0_UART0_IRQn, 1);
    NVIC_ClearPendingIRQ(UARTE0_UART0_IRQn);
    NVIC_EnableIRQ(UARTE0_UART0_IRQn);

    //=== UART1 to SCuM
    // TX: P0.26
    // RX: P0.02
    // FTDI cable:
    //     - black  GND
    //     - Orange TXD
    //     - yellow RXD
    NRF_UARTE1->RXD.PTR                = (uint32_t)app_vars.uart_buf_SCuM_RX;
    NRF_UARTE1->RXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE1->TXD.PTR                = (uint32_t)app_vars.uart_buf_SCuM_TX;
    NRF_UARTE1->TXD.MAXCNT             = UART_BUF_SIZE;
    NRF_UARTE1->PSEL.TXD               = 0x0000001a; // 0x0000001a==P0.26
    NRF_UARTE1->PSEL.RXD               = 0x00000002; // 0x00000002==P0.02
    NRF_UARTE1->CONFIG                 = 0x00000000; // 0x00000000==no flow control, no parity bits, 1 stop bit
    NRF_UARTE1->BAUDRATE               = 0x0075C000; // 0x004EA000==19200 baud (actual rate: 19208)
    NRF_UARTE1->TASKS_STARTRX          = 0x00000001; // 0x00000001==start RX state machine; read received byte from RXD register
    //  3           2            1           0
    // 1098 7654 3210 9876 5432 1098 7654 3210
    // .... .... .... .... .... .... .... ...A A: CTS
    // .... .... .... .... .... .... .... ..B. B: NCTS
    // .... .... .... .... .... .... .... .C.. C: RXDRDY
    // .... .... .... .... .... .... ...D .... D: ENDRX
    // .... .... .... .... .... .... E... .... E: TXDRDY
    // .... .... .... .... .... ...F .... .... F: ENDTX
    // .... .... .... .... .... ..G. .... .... G: ERROR
    // .... .... .... ..H. .... .... .... .... H: RXTO
    // .... .... .... I... .... .... .... .... I: RXSTARTED
    // .... .... ...J .... .... .... .... .... J: TXSTARTED
    // .... .... .L.. .... .... .... .... .... L: TXSTOPPED
    // xxxx xxxx x0x0 0x0x xxxx xx00 0xx1 x000 
    //    0    0    0    0    0    0    1    0 0x00000010
    NRF_UARTE1->INTENSET               = 0x00000010;
    NRF_UARTE1->ENABLE                 = 0x00000008; // 0x00000008==enable
    
    // enable interrupts
    NVIC_SetPriority(UARTE1_IRQn, 2);
    NVIC_ClearPendingIRQ(UARTE1_IRQn);
    NVIC_EnableIRQ(UARTE1_IRQn);
}

//=========================== interrupt handlers ==============================

void RTC0_IRQHandler(void) {

    // debug
    app_dbg.num_ISR_RTC0_IRQHandler++;

    // handle compare[0]
    if (NRF_RTC0->EVENTS_COMPARE[0] == 0x00000001 ) {
        
        // clear flag
        NRF_RTC0->EVENTS_COMPARE[0]    = 0x00000000;

        // clear COUNTER
        NRF_RTC0->TASKS_CLEAR          = 0x00000001;

        // debug
        app_dbg.num_ISR_RTC0_IRQHandler_COMPARE0++;

        // handle
        led_advance();
     }
}

void UARTE0_UART0_IRQHandler(void) {

    uint8_t uart_rx_byte;
    if (app_dbg.num_ISR_UARTE0_UART0_IRQHandler == 0) {
        app_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;
    }

    // debug
    app_dbg.num_ISR_UARTE0_UART0_IRQHandler++;

    if (NRF_UARTE0->EVENTS_ERROR == 0x00000001) {
        // clear the error and continue?
        NRF_UARTE0->EVENTS_ERROR = 0x00000000;
        NRF_UARTE0->TASKS_FLUSHRX = 0x00000001;
        NRF_UARTE0->TASKS_STARTRX = 0x00000001;
    }

    if (NRF_UARTE0->EVENTS_ENDRX == 0x00000001) {
        // byte received from DK

        // clear
        NRF_UARTE0->EVENTS_ENDRX = 0x00000000;

        // debug
        app_dbg.num_ISR_UARTE0_UART0_IRQHandler_ENDRX++;

        if(app_vars.scum_programmer_state == PROGRAMMER_WAIT_4_CMD_ST) {
            uart_rx_byte = app_vars.uart_buf_DK_RX[0];
            app_vars.uart_RX_command_buf[app_vars.uart_RX_command_idx++] = uart_rx_byte;

            if((uart_rx_byte=='\n')||(uart_rx_byte=='\r')) { // \r for debugging w/ putty, \n for the python scripts
                app_vars.uart_RX_command_idx = 0; // reset index to receive the next command
                if(memcmp(app_vars.uart_RX_command_buf,UART_TRANSFERSRAM,sizeof(UART_TRANSFERSRAM))==0) { // enter transfer SRAM state
                    app_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;
                    app_vars.uart_RX_command_idx = 0;
                    print_sram_started_msg();
                }

                else if (memcmp(app_vars.uart_RX_command_buf, UART_3WB, sizeof(UART_3WB))==0) { // enter 3WB state
                    app_vars.scum_programmer_state = PROGRAMMER_3WB_BOOT_ST;
                }
                // else - erroneous command, clear the buffer and reset to default state
                else {
                    memset(app_vars.uart_RX_command_buf,0,sizeof(app_vars.uart_RX_command_buf));
                    app_vars.uart_RX_command_idx = 0;
                    app_vars.scum_programmer_state = PROGRAMMER_WAIT_4_CMD_ST;
                }
            }
            else if (app_vars.uart_RX_command_idx > MAX_COMMAND_LEN) { // max length exceeded w/out return character, reset buffer
                memset(app_vars.uart_RX_command_buf,0,sizeof(app_vars.uart_RX_command_buf));
                app_vars.uart_RX_command_idx = 0;
                app_vars.scum_programmer_state = PROGRAMMER_WAIT_4_CMD_ST;
            }
        }
        else if (app_vars.scum_programmer_state == PROGRAMMER_SRAM_LD_ST) {
            uart_rx_byte = app_vars.uart_buf_DK_RX[0];
            app_vars.scum_instruction_memory[app_vars.uart_RX_command_idx++] = uart_rx_byte;
            if(app_vars.uart_RX_command_idx == SCUM_MEM_SIZE) { // finished w/ the memory
                // after loading memory - reset state, index, and command buffer
                print_sram_done_msg();
                app_vars.scum_programmer_state = PROGRAMMER_3WB_BOOT_ST;
                //app_vars.uart_RX_command_idx = 0;
                memset(app_vars.uart_RX_command_buf,0,sizeof(app_vars.uart_RX_command_buf));
            }
        }
        
        if (app_vars.scum_programmer_state == PROGRAMMER_3WB_BOOT_ST) {
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_CLK_PIN;
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_DATA_PIN;
            NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_EN_PIN;


            for (uint32_t i=1; i<SCUM_MEM_SIZE+1; i++) {
                for (uint8_t j=0; j<8; j++) {

                    if (((app_vars.scum_instruction_memory[i-1]>>j)&(0x01))==0x01) {
                        NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_DATA_PIN;
                    }
                    else if (((app_vars.scum_instruction_memory[i-1]>>j)&(0x01))==0x00) {
                        NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_DATA_PIN;
                    }
                    busy_wait_1us();
                    if ((i%4 == 0) && (j==7)) {
                        NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    else {
                        NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    // toggle the clock
                    busy_wait_1us();
                    NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_1us();
                    NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_1us();
                }
            }

            for (uint32_t i=1; i<10000; i++) {
                for (uint8_t j=0; j<8; j++) {
                    NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_DATA_PIN;
                    busy_wait_1us();
                    // Every 32 bits need to strobe the enable high for one cycle
                    if ((i%4 == 0) && (j==7)) {
                        NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    else {
                        NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_EN_PIN;
                    }
                    // toggle the clock
                    busy_wait_1us();
                    NRF_P0->OUTSET = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_1us();
                    NRF_P0->OUTCLR = (0x00000001) << PROGRAMMER_CLK_PIN;
                    busy_wait_1us();
                }
            }

            // after bootloading - go to print state, command buffer, and index
            print_3wb_done_msg();
            app_vars.scum_programmer_state = PROGRAMMER_SRAM_LD_ST;
            app_vars.uart_RX_command_idx = 0;
            memset(app_vars.uart_RX_command_buf,0,sizeof(app_vars.uart_RX_command_buf));
        }
    }
}

void print_3wb_done_msg(void) {
    uint8_t i;
    uint8_t j=0;
    for (i=0;i<PROG_3WB_DONE_MSG_LEN;i++) {
        app_vars.uart_buf_DK_TX[0] = PROG_3WB_DONE_MSG[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_1ms(); //TODO: this should not be necessary... but it is?
        while (NRF_UARTE0->EVENTS_TXSTARTED==0x00000000);
    }
}
void print_sram_started_msg(void) {
    uint8_t i;
    for (i=0;i<SRAM_LOAD_START_MSG_LEN;i++) {
        app_vars.uart_buf_DK_TX[0] = SRAM_LOAD_START_MSG[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_1ms(); //TODO: this should not be necessary... but it is?
        while (NRF_UARTE0->EVENTS_ENDTX==0x00000000);
    }
}
void print_sram_done_msg(void) {
    uint8_t i;
    for (i=0;i<SRAM_LOAD_DONE_MSG_LEN;i++) {
        app_vars.uart_buf_DK_TX[0] = SRAM_LOAD_DONE_MSG[i];
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        busy_wait_1ms(); //TODO: this should not be necessary... but it is?
        while (NRF_UARTE0->EVENTS_ENDTX==0x00000000);
    }
}

void UARTE1_IRQHandler(void) {

    // debug
    app_dbg.num_ISR_UARTE1_IRQHandler++;

    if (NRF_UARTE1->EVENTS_ENDRX == 0x00000001) {
        // byte received from SCuM

        // clear
        NRF_UARTE1->EVENTS_ENDRX = 0x00000000;

        // debug
        app_dbg.num_ISR_UARTE1_IRQHandler_ENDRX++;

        // send byte to DK
        app_vars.uart_buf_DK_TX[0] = app_vars.uart_buf_SCuM_RX[0];

        // start sending
        NRF_UARTE0->EVENTS_TXSTARTED = 0x00000000;
        NRF_UARTE0->TASKS_STARTTX = 0x00000001;
        while (NRF_UARTE0->EVENTS_TXSTARTED == 0x00000000);
    }
}

