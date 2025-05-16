#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "optical.h"
#include "radio.h"
#include "rftimer.h"
#include "scm3c_hw_interface.h"
#include "scum.h"

// Number of for loop cycles between Hello World messages.
// 700000 for loop cycles roughly correspond to 1 second.
#define NUM_CYCLES_BETWEEN_TX (1000000UL)
#define NUM_CYCLES_BETWEEN_PACKET (100UL)
#define TX_PACKET_LEN (64UL)

void tx_endframe_callback(uint32_t timestamp);
uint8_t packet[TX_PACKET_LEN] = {0};

int main(void) {
uint8_t fine_code = 0;
uint8_t mid_code = 0;

int main(void) {

    printf("Radio Transmitter Example\n");
    radio_init();
    LC_FREQCHANGE(0,0,0);
    radio_setEndFrameTxCb(tx_endframe_callback);
    radio_loadPacket(packet, TX_PACKET_LEN + 2);
    radio_txEnable();
    busy_wait_cycles(NUM_CYCLES_BETWEEN_PACKET);
    radio_txNow();

    while (1) {
        __WFE();
    }
}

void tx_endframe_callback(uint32_t timestamp) {
    radio_rfOff();
    fine_code += 1;
    if (fine_code > 31) {
        fine_code = 0;
	mid_code ++;
    }
    if (mid_code > 8) {
	mid_code = 0;
    }

    LC_FREQCHANGE(0,0,fine_code);

    radio_loadPacket(packet, TX_PACKET_LEN + 2);
    radio_txEnable();
    busy_wait_cycles(NUM_CYCLES_BETWEEN_PACKET);
    radio_txNow();
}
