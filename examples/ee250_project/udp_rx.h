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
#include "mutex.h"

typedef struct {
    mutex_t *mutex;
    uint32_t udp_port;
} udp_rx_args_t;

kernel_pid_t udp_rx_init(void *args);