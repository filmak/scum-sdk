#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scum.h"
#include "optical.h"
#include "rftimer.h"
#include "memory_map.h"

static void rftimer_cb_timer2(void) {
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 100000, 0);
    printf("fired timer 0\n");
}

int main(void) {
    perform_calibration();

    puts("Sample application for RFTimer");

    
    *RFTIMER_REG__COMPARE0_CONTROL_ADDR = RFTIMER_COMPARE_ENABLE | RFTIMER_COMPARE_INTERRUPT_ENABLE;
    *RFTIMER_REG__COMPARE0_ADDR = (unsigned int)(RFTIMER_REG__COUNTER) + (unsigned int)(100000); 



    rftimer_init();
    rftimer_enable_interrupts_by_id(2);
    rftimer_set_callback_by_id(rftimer_cb_timer2, 0);

    printf("%ld\n",rftimer_readCounter());
    rftimer_setCompareIn_by_id(rftimer_readCounter() + 1000, 2);
     
//    printf("%ld\n",rftimer_readCounter());
//    printf("%ld\n",rftimer_readCounter());
    printf("%ld\n",rftimer_readCounter());
    
    while (1) {
        __WFI();
    }
}

