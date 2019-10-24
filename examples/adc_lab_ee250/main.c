/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 * @brief       Lab 09: ADC and Driver Hacking
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

#include <stdio.h>

#include "xtimer.h"
#include "timex.h"
#include "periph/adc.h"

/* Only line 5 can be used because only PA pins are able to be used as ADC as
   per the CC2538's user guide (the current RIOT code is incorrect!) */
#define ADC_LINE_SELECT     5   //do not modify
#define RES                 ADC_RES_10BIT
#define DELAY               (100LU * US_PER_MS) /* 100 ms, US_PER_MS defined in timex.h */

int main(void)
{
    /* get current timestamp */
    xtimer_ticks32_t last = xtimer_now();

    /* a good practice is to initialize a variable to 0 */
    int sample = 0;

    puts("\nEE 250L Lab 09: ADC and Driver Hacking\n");

    /**
     * We can only initialize line 5 which corresponds to the Cortex M3's Port A
     * Pin 7, or "PA7" on a schematic.  We found out this mapping by looking at
     * the OpenMote-b's periph_conf.h file:
     *
     * RIOT-EE250/boards/openmote-b/include/periph_conf.h
     * 
     * The PA7 pin is exposed and available on the OpenMote-b board. To better 
     * understand the pins available on the OpenMote-b board, cross-reference 
     * the board with the schematic (look for EXT_CONN):
     *
     * https://drive.google.com/file/d/1-nyGdwE2rrjEIpIzuDLM1fcsMmyz2K-K/view?usp=sharing
     */
    if (adc_init(ADC_LINE(ADC_LINE_SELECT)) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_LINE_SELECT);
        return 1;
    } else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_LINE_SELECT);
    }

    while (1) {
        sample = adc_sample(ADC_LINE(ADC_LINE_SELECT), RES);
        if (sample < 0) {
            printf("ADC_LINE(%u): Error with line and resolution selection\n", ADC_LINE_SELECT);
        } else {
            printf("ADC_LINE(%u): %i\n", ADC_LINE_SELECT, sample);
        }
        xtimer_periodic_wakeup(&last, DELAY);
    }

    return 0;
}
