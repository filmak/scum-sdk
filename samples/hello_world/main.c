#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "helpers.h"

// Number of for loop cycles between Hello World messages.
// 700000 for loop cycles roughly correspond to 1 second.
#define NUM_CYCLES_BETWEEN_TX (1000000UL)

int main(void) {
    puts("\nWelcome to SCuM!\n");
    uint32_t g_tx_counter = 0;

    while (1) {
        printf("Hello World! %lu\n", g_tx_counter++);

        // Configure the cmake build with -DCMAKE_BUILD_TYPE=Debug to trigger this
        // assert which will print the line and file and then cause a hard fault.
        // The default build type is MinSizeRel, which will not trigger this assert.
        assert(g_tx_counter < 10);

        busy_wait_cycles(NUM_CYCLES_BETWEEN_TX);
    }
}
