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
#include "udp_rx.h"
#include "timex.h"
#include "mutex.h"
#include "random.h"
#include "xtimer.h"
#include "periph/pm.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "periph/adc.h"

#ifdef OPENMOTE_BUILD
#include "cc2538_rf.h"
#endif

#define ENABLE_DEBUG (1)
#include "debug.h"

#define TESTING                 1           /* set to 1 for testing */

#define PRNG_INITVAL            768345
#define SLEEP_MSG              "sleep"
#define SLEEP_MSG_LEN           5
#define SPAM_MSG                "spam"
#define SPAM_MSG_LEN            4
#define ADC_MSG                 "adc"
#define ADC_MSG_LEN             3
#define SPAM_CHAN               25
#define COORDINATOR_CHAN        26

/* TODO: setup udp receive thread stack and msg queue */


kernel_pid_t network_iface_pid;

/* Only line 5 can be used because only PA pins are able to be used as ADC as
   per the CC2538's user guide (the current RIOT code is incorrect!) */
#define ADC_LINE_SELECT     (5)   //do not modify
#define ADC_RES             ADC_RES_12BIT

/**
 * @brief Sends a UDP packet (do not edit this).
 *
 * @param[in] addr_str      Destination IPv6 address as a string.
 * @param[in] port_str      Destination port as a string.
 * @param[in] data          Pointer to data (must be a null terminated string).
 */
static void udp_send(char *addr_str, char *port_str, char *data)
{
    int iface;
    uint16_t port;
    ipv6_addr_t addr;

    /* get interface, if available */
    iface = ipv6_addr_split_iface(addr_str);
    if ((iface < 0) && (gnrc_netif_numof() == 1)) {
        iface = gnrc_netif_iter(NULL)->pid;
    }
    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("Error: unable to parse destination address");
        return;
    }
    /* parse port */
    port = atoi(port_str);
    if (port == 0) {
        puts("Error: unable to parse destination port");
        return;
    }

    gnrc_pktsnip_t *payload, *udp, *ip;
    unsigned payload_size;
    /* allocate payload */
    payload = gnrc_pktbuf_add(NULL, data, strlen(data), GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        puts("Error: unable to copy data to packet buffer");
        return;
    }
    /* store size for output */
    payload_size = (unsigned)payload->size;
    /* allocate UDP header, set source port := destination port */
    udp = gnrc_udp_hdr_build(payload, port, port);
    if (udp == NULL) {
        puts("Error: unable to allocate UDP header");
        gnrc_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &addr);
    if (ip == NULL) {
        puts("Error: unable to allocate IPv6 header");
        gnrc_pktbuf_release(udp);
        return;
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
        return;
    }
    /* access to `payload` was implicitly given up with the send operation above
     * => use temporary variable for output */
    DEBUG("Success: sent %u byte(s) to [%s]:%u\n", payload_size, addr_str,
           port);
}

static void radio_sleep(void) {
    puts("Turning off radio...");
#ifdef OPENMOTE_BUILD
    cc2538_off();
    xtimer_usleep(4000000);
    cc2538_on();
#endif 
    puts("Turning on radio...");
    return;
}

static void spam_packets(uint16_t port) {
    /* TODO: send 200 packets to the ipv6 multicast address ff02::1 with a 
    payload of 50 bytes using udp_send() with an interval of 10 ms. we use the
    multicast address as a substitute for the imaginary server. go ahead and 
    hardcode it here. */ 
    puts("Spamming packets...");
    uint16_t spam_chan = SPAM_CHAN; 
    uint16_t coordinator_chan = COORDINATOR_CHAN;
    gnrc_netapi_set(network_iface_pid, NETOPT_CHANNEL, 0, &spam_chan, sizeof(spam_chan));
    gnrc_netapi_get(network_iface_pid, NETOPT_CHANNEL, 0, &spam_chan, sizeof(spam_chan));
    DEBUG("Channel now set to %d\n", spam_chan);


    puts("Finished spamming packets!");
    gnrc_netapi_set(network_iface_pid, NETOPT_CHANNEL, 0, &coordinator_chan, sizeof(spam_chan));
    return;
}

static void repeat_adc_sample(void) {
    /* TODO: make this function sample the ADC 50 times within 4 seconds. Do 
       not modify the code inside the #ifdef-#endif statement */
    int sample;

    #ifdef OPENMOTE_BUILD    /* native does not have an ADC */
        sample = adc_sample(ADC_LINE(ADC_LINE_SELECT), ADC_RES);
        if (sample < 0) {
            printf("ADC_LINE(%u): Error with line and resolution selection\n", ADC_LINE_SELECT);
        } else {
            printf("ADC_LINE(%u): %i\n", ADC_LINE_SELECT, sample);
        }
    #else
        /* adc_init() not applicable for native */
        sample = 1024;
        printf("fake ADC value for native: %d\n", sample);  
    #endif 
    
    return;
}

static void *_udp_rx(void *arg)
{
    /* TODO: parse arg by typecasting it into the correct struct type */

    gnrc_pktsnip_t *snip;
    msg_t msg, reply; 
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;
    reply.content.value = -ENOTSUP;

    /* TODO: initialize msg queue. Create any global symbols as needed. */

    gnrc_netreg_entry_t udp_rx_reg;
    /* TODO: fill the contents of udp_rx_reg with the port number and 
       thread ID of the thread running this function. Then register this thread
       with the networking stack. For help, see the example code in "gnrc.h". */

#if TESTING
    spam_packets(4321);
    repeat_adc_sample();
    radio_sleep();
    return 0;
#endif

    while (1) {
        /* TODO: receive a msg_t */

        /* We build off the general structure for designing a loop in a thread
           receiving network packets outlined in gnrc.h */
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("udp_rx_thr: data received\n");

                /* TODO: if this is a network packet, msg.content.ptr points to 
                   a node of type gnrc_pktstnip_t in a linked list. Each node
                   on the linked list (i.e. "snip")  represents a different 
                   section of a network packet. More specifically, for a UDP 
                   packet, there will be a node for each of the following: MAC 
                   layer (802.15.4) header, ethernet header, ipv6 header, udp 
                   header, and udp payload. More information on the linked list
                   structure can be found in net/gnrc/pkt.h. Your job is to
                   extract the payload of the packet, which will contain the
                   command sent by the coordinator. Hint: the protocol for the
                   snip containing the payload is GNRC_NETTYPE_UNDEF. It is an 
                   odd name choice, but this was determined by RIOT developers. 

                   Once you find the payload, you will need to parse the payload
                   and either sleep, spam, or sample the ADC. When you're done
                   with the packet, make sure to tell the network stack you're
                   done with it so it can free up its resources! More hints can
                   be found in the documentaiton in net/gnrc.h.
                   */

                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("udp_rx_thr: sending packets is not my job even though the \
                    way the library is built forwards this message type. \
                    ignoring.\n");
                gnrc_pktbuf_release(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
                DEBUG("udp_rx_thr: get commands are not my job. ignoring.\n");
                break;
            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("udp_rx_thr: set commands are not my job. ignoring w/ the \
                    necessary reply as per msg.h specifications.\n");
                msg_reply(&msg, &reply);
                break;
            default:
                DEBUG("udp_rx_thr: ERROR - received something unexpected");
                break;
        }
    }

    /* this should never be reached */
    gnrc_netreg_unregister(GNRC_NETTYPE_UDP, &udp_rx_reg); //not necessary for neverending threads

    return 0;
}

void udp_rx_register_iface(kernel_pid_t pid) {
    network_iface_pid = pid;
}

kernel_pid_t udp_rx_init(void *args)
{
    /* TODO: create the thread and return the new thread ID. make sure to pass 
       the input to this function to the new thread. we do this to make this 
       thread design more generic so the thread parameters can be application 
       specific and defined in main.c. use the same thread stacksize as main. 
       Lastly, return the pid of the new thread. */
}
