/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx.c
 * @brief       UDP receiver thread
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "udp_rx.h"
#include "timex.h"
#include "mutex.h"
#include "random.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

//TODO
#define PRNG_INITVAL            0  //TODO: please use a unique int
#define SLEEP_MSG_STR           ""
#define SLEEP_MSG_LEN           0

#define SLEEP_INTERVAL_SECS     (0)
#define RAND_USLEEP_MIN         (0)
#define RAND_USLEEP_MAX         (1000000)

static void *_udp_rx(void *arg)
{
    udp_rx_args_t *udp_rx_args = (udp_rx_args_t *)arg; 

    /* initialize PRNG */
    random_init(PRNG_INITVAL);
    printf("PRNG initialized to current time: %d\n", PRNG_INITVAL);

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp_rx: data received\n");
                // TODO: if(msg size is valid and msg includes sleep string){
                
                    
                    /* additional random sleep to reduce network collisions */
                    uint32_t interval = random_uint32_range(RAND_USLEEP_MIN, RAND_USLEEP_MAX);
                    xtimer_usleep(interval);

                // }
                gnrc_pktbuf_release(msg.content.ptr);
                break;

            //TODO


            default:
                DEBUG("udp_rx_thr: received something unexpected");
                break;
        }
    }

    /* should never be reached */
    DEBUG("ERROR!\n")
    return 1;
}

kernel_pid_t udp_rx_init(void *args)
{
    // return udp_rx_pid;
}