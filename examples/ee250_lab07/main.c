/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 * @brief       Lab 07: Receive 10 UDP packets and quit.
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

/** We've provided all the includes you need. Any functions you see should be 
 * defined in these header files, so browse these header files and their 
 * corresponding implementations (often the equivalent .c file) if you need more
 * information on something
 */
#include <stdio.h>

#include "msg.h"
#include "xtimer.h"
#include "udp_rx_thr.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
 
/* Print statements always slow code down. Also, sometimes you don't need all
   print statements unless you are debugging. Use DEBUG() instead of printf() if
   you want to disable the print statements on the fly by setting ENABLE_DEBUG 
   to 0 */
#define ENABLE_DEBUG (0)
#include "debug.h"

/* Use macros to give a constant a name. Arbitrary numbers in code is usually
   bad practice */
#define MAIN_QUEUE_SIZE     (8)
#define NUM_PKTS_TO_RX      (10)
#define UDP_PORT            (8050)

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

kernel_pid_t main_pid = KERNEL_PID_UNDEF;

int main(void)
{
    msg_t msg;
    msg_init_queue(main_msg_queue, sizeof(main_msg_queue));

    printf("Welcome to Lab 07!\n");

    /* Code to simply print out the RIOT device's IPv6 address */
    gnrc_netif_t *netif = NULL;
    while ((netif = gnrc_netif_iter(netif))) {
        ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
        int res = gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                                  sizeof(ipv6_addrs));

        if (res < 0) {
            continue;
        }
        for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv6_addr_t)); i++) {
            char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];

            ipv6_addr_to_str(ipv6_addr, &ipv6_addrs[i], IPV6_ADDR_MAX_STR_LEN);
            printf("My address is %s\n", ipv6_addr);
        }
    }

    /* TODO: Initialize udp_rx_thr with the custom struct type that defines the 
    arguments in which udp_rx_thr should use to run */
    udp_rx_thr_init((void *) &udp_rx_args); 
    
    while(1)
    {
        msg_receive(&msg);

        if (msg.type == UDP_RX_DONE) {
            printf("main: received shutdown signal from udp_rx_thr\n");
            return 0; // when main exits, RIOT-OS shuts down
        }
 
        DEBUG("main: received illegal message...\n");
        xtimer_usleep(1000000); //sleep just in case we rcv illegal msgs
    }

    /* should never be reached */
    DEBUG("ERROR!\n");
    return -1; 
}
