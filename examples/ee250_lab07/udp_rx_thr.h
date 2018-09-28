/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx_thr.h
 * @brief       UDP receiver thread header
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

/* Header files are used to share global functions and variables. */

#include "kernel_types.h"

#define UDP_RX_DONE     9 //arbitrary number

typedef struct {
    kernel_pid_t main_pid;
    unsigned int num_pkts;
    uint32_t udp_port;
} udp_rx_args_t;

kernel_pid_t udp_rx_thr_init(void *args);