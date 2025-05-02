#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scum.h"
#include "optical.h"
#include "rftimer.h"
#include "memory_map.h"

#define RFTIMER_CHANNEL 2U

static void rftimer_cb2( void ) {
    puts("a");
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 100000, RFTIMER_CHANNEL);
}

int main(void) {
    perform_calibration();

    puts("Sample application for RFTimer");

    rftimer_init();
    //rftimer_enable_interrupts();
    rftimer_enable_interrupts_by_id(RFTIMER_CHANNEL);
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 100000, RFTIMER_CHANNEL);

    rftimer_set_callback_by_id(rftimer_cb2, RFTIMER_CHANNEL);

 
    while (1) {
        __WFI();
    }
}

