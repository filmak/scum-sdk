#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "optical.h"
#include "radio.h"
#include "rftimer.h"
#include "scm3c_hw_interface.h"

// Number of for loop cycles between Hello World messages.
// 700000 for loop cycles roughly correspond to 1 second.
#define NUM_CYCLES_BETWEEN_TX (1000000UL)
#define NUM_CYCLES_BETWEEN_PACKET (100UL)
#define TX_PACKET_LEN (64UL)

void tx_endframe_callback(uint32_t timestamp);

uint8_t packet[TX_PACKET_LEN] = {0};


int main(void) {
    perform_calibration();

    LC_FREQCHANGE(0,0,0);
    radio_txEnable();
    radio_setEndFrameTxCb(tx_endframe_callback);
    send_packet(packet, TX_PACKET_LEN + 2);

    uint32_t g_tx_counter = 0;
    while (1) {
        printf("Hello World! %lu\n", g_tx_counter++);
        busy_wait_cycles(NUM_CYCLES_BETWEEN_TX);
    }
}

void tx_endframe_callback(uint32_t timestamp) {

    printf("sent a packet\r\n");
    send_packet(packet, TX_PACKET_LEN + 2);
}
