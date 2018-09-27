/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Lab 07: Receive 10 UDP packets and quit.
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"
 
#define ENABLE_DEBUG (0)
#include "debug.h"

#define MAIN_QUEUE_SIZE     (8)

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    msg_t msg;
    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Welcome to Lab 07!");

    // kernel_pid_t udp_rx_thr_pid = ??
    
    while(1)
    {
        msg_receive(&msg);

        if (msg.content.value == 1) {
            DEBUG("main: received shutdown signal from udp_rx_thr\n");
            return 0;
        }

        xtimer_usleep(1000000); //sleep just in case we rcv illegal msgs
    }

    /* should never be reached */
    return -1; //ERROR!
}
