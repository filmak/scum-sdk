#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scum.h"
#include "optical.h"
#include "rftimer.h"

#define RFTIMER_CHANNEL (1U)
#define RFTIMER_DELAY   (1000000UL)

static void rftimer_cb2(void) {
    rftimer_setCompareIn_by_id(rftimer_readCounter() + RFTIMER_DELAY, RFTIMER_CHANNEL);
    printf("RFTimer callback %d triggered - %lu\n\n", RFTIMER_CHANNEL, rftimer_readCounter());
}

int main(void) {
    perform_calibration();

    puts("Sample application for RFTimer");

    rftimer_init();
    rftimer_set_callback_by_id(rftimer_cb2, RFTIMER_CHANNEL);
    rftimer_setCompareIn_by_id(rftimer_readCounter() + RFTIMER_DELAY, RFTIMER_CHANNEL);

    while (1) {
        puts("Waiting for interrupt...");
        __WFI();
    }
}
