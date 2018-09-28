/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx_thr.c
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
#include "cc2538_rf.h"

/**
 * TODO (Lab 08): implement this functino for the RSS and PRR lab assignment
 *
 * For each packet you receive, you need to extract the Received Signal Strength
 * Indicator (RSSI) from the layer 2 header. The Received Signal Strength (no I)
 * is equal to the RSSI minus CC2538_RSSI_OFFSET. You must cast the original 
 * RSSI value because it is a 2-complement number. This function should 
 * ultimately print out the RSS of the packet inputted.
 */
static void print_rss(gnrc_pktsnip_t *pkt)
{

}

/**
 * TODO (Lab 08): implement this function for the RSS and PRR lab assignment
 *
 * You know the number of packets you need to receive, and how many packets
 * you actually received. Calculate the Packet Reception Ratio and print it
 * out.
 */
static void print_prr(uint32_t pkt_rcv, uint32_t num_pkts)
{

}

/**
 * TODO: implement this function 
 * 
 * When a packet is received, GNRC will give you the packet as a linked list
 * of snips. Look for the snip of type GNRC_NETTYPE_UNDEF. The data here is
 * the packet's payload. Print the payload. You can assume they are readable
 * ascii characters but you can NOT assume the payload is a null terminated
 * string. Thus, you should print based on the size of the data. FYI, this
 * linked list is not circular. You can us gnrc_pktsnip_search_type().
 */
static void _print_payload(gnrc_pktsnip_t *pkt)
{

}

static void *_udp_rx_thr(void *arg)
{
    udp_rx_args_t *udp_rx_args = (udp_rx_args_t *)arg; //cast it to the right type to parse
    kernel_pid_t main_pid = udp_rx_args->main_pid;
    int num_pkts = udp_rx_args->num_pkts;
    uint32_t udp_port = udp_rx_args->udp_port;

    msg_t msg, reply; 

    //TODO: what is `msg_t reply` used for? see the documentation in gnrc.h. 
    //This is a weird quirk of the RIOT-OS kernel design, so we have to include it.

    int rcvd_all_pkts = 0;

    /* TODO: create and init this thread's msg queue for async messages */

    /* TODO: register this thread to UDP packets coming in on udp_port */

    /* TODO: exit the while loop when num_pkts packets have been received */
    while (!rcvd_all_pkts) {
        msg_receive(&msg);

        /* Use gnrc_pktdump.c as a model and refer to gnrc.h for documentation 
           on how to structure this thread. If you receive a pointer to a packet
           make sure to gnrc_pktbuf_release(msg.content.ptr) to free up space!
           This is not explicitly stated in gnrc.h but you can see how it's
           done in gnrc_pktdump.c. */
        switch (msg.type) {
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
      what will happen to the input argument `args`. 
    */ 

    /* use thread_create() here */
}