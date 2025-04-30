#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scum.h"
#include "optical.h"
#include "rftimer.h"
#include "memory_map.h"

static void rftimer_cb2( void ) {
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 100000, 2);
}

int main(void) {
    perform_calibration();

    puts("Sample application for RFTimer");

    rftimer_init();
    rftimer_enable_interrupts();
    rftimer_enable_interrupts_by_id(2);
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 100000, 2);

    rftimer_set_callback_by_id(rftimer_cb2, 2);

 
    //while (1) {
    //    __WFI();
    //}
}

