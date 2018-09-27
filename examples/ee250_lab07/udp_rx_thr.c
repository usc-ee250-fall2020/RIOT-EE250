/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_pktdump
 * @{
 *
 * @file
 * @brief       Generic module to dump packages received via netapi to STDOUT
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "udp_rx_thr.h"

static void print_rss(msg_t *msg)
{
    if (msg->type == GNRC_NETAPI_MSG_TYPE_RCV) {

        gnrc_pktsnip_t *pkt = msg->content.ptr;
        gnrc_pktsnip_t *snip = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);
        gnrc_netif_hdr_t *hdr = snip->data;

        /* *** TO-DO *** */

        /* gnrc_netif_hdr_t has information about the signal quality */
        /* The RSS value is equal to the RSSI from 'hdr' minus CC2538_RSSI_OFFSET */
        /* You must cast the RSSI from 'hdr' to signed integer because it is a 2-complement number */
        /* You have to print out the RSS of each packet received */
        


        /* Tell GNRC you are done with this packet so it can release the memory */
        gnrc_pktbuf_release(pkt);
    }
}

static void print_prr(uint32_t pkt_rcv, uint32_t num_pkts)
{
    /* *** TO-DO *** */

    /* You know the number of packets you were supposed to receive */
    /* and the number of packets you actually received. */
    /* So, calculate what is the the Packet Reception Ratio and print out */

}

static void _print_payload(gnrc_pktsnip_t *pkt)
{
    /* when the lower layers of RIOT's "gnrc" network stack receives a packet,
     it will deconstruct the bytes and give you the packet in a linked list of
     what they call "snips." Each "snip" is a struct of type gnrc_pktsnip_t. This
     struct gives you a pointer to data and the size of the data array in bytes.
     Do NOT assume the data is a null terminated string. The snip of type
     NETTYPE_UNDEF is where the payload lies. The linked list is NOT circular.*/ 
}

static void *_udp_rx_thr(void *arg)
{
    udp_rx_args_t *udp_rx_args = (udp_rx_args_t *)arg;  
    kernel_pid_t main_pid = udp_rx_args->main_pid;
    int num_pkts = udp_rx_args->num_pkts;
    uint32_t udp_port = udp_rx_args->udp_port;

    msg_t msg, reply; //TODO: why did we define `reply` for you? see gnrc.h.

    //TODO: what is `msg_t reply` used for? see gnrc.h. This is a weird quirk of
    //the RIOT-OS kernel design

    int rcvd_all_pkts = 0;
    int num_rcvd = 0;

    /* TODO: intialize msg queue for async messages */

    /* TODO: register this thread to UDP packets coming in on udp_port */

    /* TODO: exit the while loop when num_pkts packets have been received */
    while (!rcvd_all_pkts) {
        /* delete this comment after you address it! */
        /* todo: to recv msgs, what do you need to setup? */
        msg_receive(&msg);

        /* use gnrc_pktdump.c as a model and refer to pkt.h for documentation */
        /* if you receive a pointer to a packet, make sure to 
        gnrc_pktbuf_release(msg.content.ptr) to free up space! This is not
        accurately represented in the pkt.h documentation, but is called in 
        _dump_snip() in gnrc_pktdump.c. */
        switch (msg.type) {
            //case ??:
            //case ??:
            default:
                break;
        }
    }

    /* TODO: Unregister this thread from UDP packets. Technically unnecessary, 
    but let's do it for completion and good practice. */

    /* send shutdown message to main thread */
    msg.type = NULL; //we are not using this field
    //msg.content.value = ?? //TODO
    msg_send(&msg, main_pid);

    return 0;
}

kernel_pid_t udp_rx_thr_init(void *args)
{
    /* What is `args` supposed to be?!

    C coders use a pointer to void to tell programmers that this pointer 
    argument can point to anything you need. It could be a pointer to
    a function, variable, struct, etc. This allows for code *flexibility*. 
    When this is called in main(), we cast a pointer to udp_rx_args to (void *) 
    because we know what this pointer actually points to since udp_rx_args_t is 
    a type specific to udp_rx_thr.c/.h. That is, if you are calling 
    udp_rx_thr_init(), you have already read this file which tells shows you
    what will happen to the input argument `args`. */ 

    /* use thread_create() here. You will need a statically allocated stack to
    create a thread. Also, you should pass args to the thread function 
    (i.e., _udp_rx_thr()) */
}