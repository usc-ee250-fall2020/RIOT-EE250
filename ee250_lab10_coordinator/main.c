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
 * @brief       EE 250L Lab 09: Instructor Multicaster
 *
 * @author      Jason A. Tran <jasontra@usc.edu>
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
#include "net/gnrc.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"

/* Print statements always slow code down. Also, sometimes you don't need all
   print statements unless you are debugging. Use DEBUG() instead of printf() if
   you want to disable the print statements on the fly by setting ENABLE_DEBUG 
   to 0 */
#define ENABLE_DEBUG (1)
#include "debug.h"

/* Use macros to give a constant a name. Arbitrary numbers in code is usually
   bad practice */
#define MAIN_QUEUE_SIZE     (8)
#define NUM_PKTS_TO_TX      (0)
#define UDP_SRC_PORT        (9000)
#define UDP_DST_PORT        (8000)
#define PKT_INTERVAL_USEC   (5000000)
#define TX_POWER            (7) /* dBm */

#define SLEEP_MSG_STR       "sleep"
#define SLEEP_MSG_LEN       5
#define SPAM_MSG_STR        "spam"
#define SPAM_MSG_LEN        4
#define ADC_MSG_STR         "adc"
#define ADC_MSG_LEN         3

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

kernel_pid_t main_pid = KERNEL_PID_UNDEF;

int main(void)
{
    msg_init_queue(main_msg_queue, sizeof(main_msg_queue));

    printf("Lab 08 Multicaster Code.\n");

    /* Code to simply print out the RIOT device's IPv6 address */
    gnrc_netif_t *netif = NULL;
    int16_t tx_power = TX_POWER;
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

            /* For EE 250L, we only use devices with one netif, se it's safe to set th elappower now */
            gnrc_netapi_set(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            gnrc_netapi_get(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            printf("Power level is now %d\n", tx_power);
        }
    }

    int iface;
    char *addr_str = "ff02::1";
    ipv6_addr_t addr;
    uint16_t port = UDP_DST_PORT;
    char data_to_send[3][5] = {
        {'s', 'p', 'a', 'm'},
        {'s', 'l', 'e', 'e', 'p'},
        {'a', 'd', 'c'}
    };
    unsigned int data_to_send_len[3] = {SPAM_MSG_LEN, SLEEP_MSG_LEN, ADC_MSG_LEN};
    unsigned int num_pkts = NUM_PKTS_TO_TX;

    /* get interface, if available */
    iface = ipv6_addr_split_iface(addr_str);
    if ((iface < 0) && (gnrc_netif_numof() == 1)) {
        iface = gnrc_netif_iter(NULL)->pid;
    }

    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return 1;
    }

    for (unsigned int i = 0; i < num_pkts || num_pkts == 0; i++) {
        gnrc_pktsnip_t *payload, *udp, *ip;
        unsigned payload_size;

        /* allocate payload */
        payload = gnrc_pktbuf_add(NULL, data_to_send[i % 3], data_to_send_len[i % 3], GNRC_NETTYPE_UNDEF);
        if (payload == NULL) {
            puts("Error: unable to copy data to packet buffer");
            return 1;
        }

        for (unsigned int j = 0; j < data_to_send_len[i % 3]; j++) {
            printf("%c", data_to_send[i % 3][j]);
        }

        printf("\n");

        /* store size for output */
        payload_size = (unsigned)payload->size;

        /* allocate UDP header, set source port := destination port */
        udp = gnrc_udp_hdr_build(payload, UDP_DST_PORT, UDP_DST_PORT);
        if (udp == NULL) {
            puts("Error: unable to allocate UDP header");
            gnrc_pktbuf_release(payload);
            return 1;
        }

        /* allocate IPv6 header */
        ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
        if (ip == NULL) {
            puts("Error: unable to allocate IPv6 header");
            gnrc_pktbuf_release(udp);
            return 1;
        }

        /* add netif header, if interface was given */
        if (iface > 0) {
            gnrc_pktsnip_t *netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);

            ((gnrc_netif_hdr_t *)netif->data)->if_pid = (kernel_pid_t)iface;
            LL_PREPEND(ip, netif);
        }

        /* send packet */
        if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
            puts("Error: unable to locate UDP thread");
            gnrc_pktbuf_release(ip);
            return 1;
        }

        DEBUG("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str, port);

        xtimer_usleep(PKT_INTERVAL_USEC);
    }

    return 0; 
}
