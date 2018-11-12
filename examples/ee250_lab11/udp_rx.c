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
#include "udp_rx_thr.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static void *udp_rx(void *arg)
{
    udp_rx_args_t *udp_rx_args = (udp_rx_args_t *)arg; //cast it to the right type to parse
    kernel_pid_t main_pid = udp_rx_args->main_pid;
    int num_pkts = udp_rx_args->num_pkts;
    uint32_t udp_port = udp_rx_args->udp_port;

    while (!rcvd_all_pkts) {
        msg_receive(&msg);

        switch (msg.type) {
            //case ??:
            default:
                DEBUG("udp_rx_thr: received something unexpected");
                break;
        }
    }

    /* TODO: Unregister this thread from UDP packets. Technically unnecessary, 
    but let's do it for completion and good practice. */

    return 0;
}

kernel_pid_t udp_rx_init(void *args)
{
    //TODO
}