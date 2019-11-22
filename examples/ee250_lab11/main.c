/**
 * @ingroup     examples
 * @{
 *
 * @file        main.c
 * @brief       Lab 11: Final Project.
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
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
#include "mutex.h"
#include "random.h"
 
#define ENABLE_DEBUG (1)
#include "debug.h"

#define MAIN_QUEUE_SIZE     (8)
#define TX_POWER            (7)
#define UDP_SEND_PORT_STR   "0000"  //TODO
#define UDP_RX_PORT          0      //TODO

/* Only line 5 can be used because only PA pins are able to be used as ADC as
   per the CC2538's user guide (the current RIOT code is incorrect!) */
#define ADC_LINE_SELECT     (5)   //do not modify
#define ADC_RES             ADC_RES_12BIT

#define RELAY_NODE_ADDR     "TO:DO" //TODO

#define SENSE_INTERVAL_SEC  (0)

static msg_t main_msg_queue[MAIN_QUEUE_SIZE];

/**
 * @brief Sends a UDP packet
 *
 * @param[in] addr_str      Destination IPv6 address as a string.
 * @param[in] port_str      Destination port as a string.
 * @param[in] data          Pointer to data. Must be a null terminated string 
 *                          but will not send the null character.
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

int main(void)
{
    int sample = 0;

    msg_init_queue(main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Welcome to Lab 11!");

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

            /* For EE 250L, we only use devices with one netif, se it's safe to set the power here */
            gnrc_netapi_set(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            gnrc_netapi_get(netif->pid, NETOPT_TX_POWER, 0, &tx_power, sizeof(tx_power));
            printf("Power level is now %d\n", tx_power);
        }
    }

    // udp_rx_args_t udp_rx_args = //TODO
    udp_rx_init(/*TODO*/); 

    /**
     * We can only initialize line 5 which corresponds to the Cortex M3's Port A
     * Pin 7, or "PA7" labeled on the schematic/board.  We found out this 
     * mapping by looking at the OpenMote-b's periph_conf.h file:
     *
     * RIOT-EE250/boards/openmote-b/include/periph_conf.h
     * 
     * The PA7 pin is exposed and available on the OpenMote-b board. To better 
     * understand the pins available on the OpenMote-b board, cross-reference 
     * the board with the schematic (look for EXT_CONN):
     *
     * https://drive.google.com/file/d/1fdpE-hnl0KNitM3-c0wZxHWbi3oVNMI7/view?usp=sharing
     */
#ifdef OPENMOTE_BUILD    //native does not have an ADC
    if (adc_init(ADC_LINE(ADC_LINE_SELECT)) < 0) {
        printf("Initialization of ADC_LINE(%u) failed\n", ADC_LINE_SELECT);
        return 1;
    } else {
        printf("Successfully initialized ADC_LINE(%u)\n", ADC_LINE_SELECT);
    }
#endif
    
    while(1)
    {
        //TODO: handle mutex, format payload, send udp pkt, set sample interval

#ifdef MODULE_PERIPH_ADC
        sample = adc_sample(ADC_LINE(ADC_LINE_SELECT), ADC_RES);
        if (sample < 0) {
            printf("ADC_LINE(%u): Error with line and resolution selection\n", ADC_LINE_SELECT);
        } else {
            printf("ADC_LINE(%u): %i\n", ADC_LINE_SELECT, sample);
        }
#else
        sample = 1024;
        printf("fake ADC value for native: %d\n", sample);  
#endif

    }

    /* should never be reached */
    DEBUG("ERROR!\n");
    return 1; 
}
