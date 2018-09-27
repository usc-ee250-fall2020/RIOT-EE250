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
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "net/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/sixlowpan.h"
#include "od.h"

/* allocate a stack for the UDP receiver thread */
static char udp_rx_thr_stack[THREAD_STACKSIZE_MAIN];

static void print_payload(gnrc_pktsnip_t *pkt)
{
    /* when the lower layers of RIOT's "gnrc" network stack receives a packet,
     it will deconstruct the bytes and give you the packet in a linked list of
     what they call "snips." Each "snip" is a struct of type gnrc_pktsnip_t. This
     struct gives you a pointer to data and the size of the data array in bytes.
     This is NOT a null terminated string, so you can NOT use DEBUG() or printf().
     Find the snip of type "NETTYPE_UNDEF" to get to the actual packet payload */ 
}

static void *_udp_rx_thr(void *arg)
{
    unsigned int num_pkts = (unsigned int)arg;  

    while (1) {
        /* delete this comment after you address it! */
        /* todo: to recv msgs, what do you need to setup? */
        msg_receive(&msg);

        switch (msg.type) {
            //case ??:
            //case ??:
            default:
                break;
        }
    }

    /* never reached */
    return NULL;
}

kernel_pid_t udp_rx_thr_init(void *num_pkts)
{
    //??
}