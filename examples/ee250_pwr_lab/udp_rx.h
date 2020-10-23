/**
 * @ingroup     examples
 * @{
 *
 * @file        udp_rx.h
 * @brief       UDP receiver thread header file
 *
 * @author      [list team members here]
 *
 * Github Link: [insert here]
 *
 * @}
 */

/* Header files are used to share global functions and variables. */

#include "kernel_types.h"

typedef struct {
    uint16_t udp_rx_port;
    uint16_t udp_send_port;
} udp_rx_args_t;

void udp_rx_register_iface(kernel_pid_t pid);

kernel_pid_t udp_rx_init(void *args);
