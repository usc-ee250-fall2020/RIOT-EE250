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
 * @brief       example application for setting a periodic wakeup
 *
 * @author      Jason A. Tran <jasontra@usc.edu>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "xtimer.h"
#include "timex.h"

#define INTERVAL (1U * US_PER_SEC)          /* 1 second */
#define PRINT_INTERVAL (10U * US_PER_SEC)   /* 10 seconds */

char producer_stack[THREAD_STACKSIZE_DEFAULT];

/* package constants into an enum type */
typedef enum {
    EVENT_1010 = 0,
    EVENT_1001,
} event_type_t; 

typedef struct {
    kernel_pid_t detectorA;
    kernel_pid_t detectorB;
} thread_pids_t;

/* a link of the linked list */
typedef struct {
    struct event_t *next;
    event_type_t type;
} event_t;

/* head of the linked list */
event_t *head = NULL;

mutex_t list_lock;

void *producer_thread(void *arg)
{
    /* use input arg to extract the other thread process IDs */
    thread_pids_t *pids = (thread_pids_t *)arg; //cast it to the right type to parse

    while(1) {
        xtimer_usleep(INTERVAL);
        printf("slept until...\n");
        
    }

    /* this should never be reached */
    return NULL;
}

void *detectorA_thread(void *arg) {
    msg_t msg;

    while (1) {
        msg_receive(&msg)

        switch (msg.type) {

        }

    }

}

void *detectorB_thread(void *arg) {
    msg_t msg;

    while (1) {
        msg_receive(&msg)

        switch (msg.type) {

        }
    }

}

int main(void)
{
    msg_t msg;

    thread_pids_t pids;

    /* TODO: create detectorA_thread and detectorB_thread */

    //pids.detectorA = thread_create(???)
    
    //pids.detectorB = thread_create(???)

    thread_create(
            producer_stack, sizeof(producer_stack), // pass in stack
            THREAD_PRIORITY_MAIN - 1,               // set priority
            THREAD_CREATE_STACKTEST,                // for debugging
            producer_thread,                        // set thread function
            (void *) &pids,                         // thread func. input arg 
            "producer_thread");                     // thread name


    /* use main thread as the final printing thread */
    while (1) {
        xtimer_usleep(PRINT_INTERVAL);

        /* TODO: check if there are any events in the linked list and print the
        sequences comma-separated followed by the total number printed in one
        loop on a new line */
    }


    thread_sleep();

    return 0;
}
