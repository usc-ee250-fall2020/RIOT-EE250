/*
 * Copyright (C) 2015 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

/* allocate a stack for the UDP receiver thread */
static char udp_rx_thr_stack[THREAD_STACKSIZE_MAIN];

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
