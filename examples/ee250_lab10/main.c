/*
 * Copyright (C) 2019 Jason A. Tran <jasontra@usc.edu>
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
 * @brief       EE 250L Lab 10: Exploring Energy Efficiency
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
 * information on something.
 */
#include <stdio.h>
#include "msg.h"
#include "xtimer.h"
#include "udp_rx.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "mutex.h"

/* Print statements always slow down embedded systems. Also, sometimes you don't 
   need all print statements unless you are debugging. Use DEBUG() instead of 
   printf() if you want to disable the print statements on the fly by setting 
   ENABLE_DEBUG to 0 */
#define ENABLE_DEBUG (1)
#include "debug.h"

#define MAIN_QUEUE_SIZE     (8)
#define TX_POWER            (0 )

#define UDP_SEND_PORT        9000   //TODO
#define UDP_RX_PORT          8000   //TODO

/* Only line 5 can be used because only PA pins are able to be used as ADC as
   per the CC2538's user guide (the current RIOT code is incorrect!) */
#define ADC_LINE_SELECT     (5)   //do not modify
#define ADC_RES             ADC_RES_12BIT

int main(void)
{
    puts("Welcome to Lab 10!");

    /* Code to simply print out the RIOT device's IPv6 address */
    gnrc_netif_t *netif = NULL;
    int16_t tx_power = TX_POWER;
    while ((netif = gnrc_netif_iter(netif))) {
        ipv6_addr_t ipv6_addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
        int res = gnrc_netapi_get(netif->pid, NETOPT_IPV6_ADDR, 0, ipv6_addrs,
                                  sizeof(ipv6_addrs));
        udp_rx_register_iface(netif->pid);

        if (res < 0) {
            continue;
        }
        for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv6_addr_t)); i++) {
            char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];

            ipv6_addr_to_str(ipv6_addr, &ipv6_addrs[i], IPV6_ADDR_MAX_STR_LEN);
            printf("My address is %s\n", ipv6_addr);

            /* For EE 250L, we only use devices with one netif, so it's safe to set the power here */
            gnrc_netapi_set(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            gnrc_netapi_get(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            printf("Power level is now %d\n", tx_power);
        }
    }

#ifdef OPENMOTE_BUILD    //native does not have an ADC
    if (adc_init(ADC_LINE(ADC_LINE_SELECT)) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_LINE_SELECT);
        return 1;
    } else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_LINE_SELECT);
    }
#endif

    /* TODO: create a thread using udp_rx_init(). Upon thread creation, pass in
       a pointer to a struct created locally in main() (since main never exits) 
       of type udp_rx_args_t. Search for the definition for more information. 
       Requirement: ports should be defined in a macro to make it easy to change
       if needed. The receive port is 8000 and the send port is 9000.
    */

    /* TODO: make the main thread go idle forever. Hint: see the thread.h API
    to find the best function to call here. */

    /* should never be reached */
    DEBUG("ERROR!\n");
    return -1; 
}
