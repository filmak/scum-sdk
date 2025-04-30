#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "scum.h"
#include "optical.h"
#include "systick.h"

static void _systick_cb(void) {
    printf("Systick counter: %20lu\n", systick_count());
}

int main(void) {
    puts("\nRunning the systick sample application!\n");

    systick_init(100000, _systick_cb);

    while (1) {
        __WFI();
    }
}
