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
 * @brief       EE 250L Lab 09: RIOT-OS Warmup
 *
 * @author      Jason A. Tran <jasontra@usc.edu>
 *
 * @}
 */

#include <stdio.h>
#include "thread.h"
#include "xtimer.h"
#include "timex.h"
#include "random.h"

/* Print statements always slow down embedded systems. Also, sometimes you don't 
   need all print statements unless you are debugging. Use DEBUG() instead of 
   printf() if you want to disable the print statements on the fly by setting 
   ENABLE_DEBUG to 0 */
#define ENABLE_DEBUG (1)
#include "debug.h"

#define PRNG_INITVAL        418 /* seed value for pseudorandom number generator */
#define PROB_OF_1           0.3 /* probability of generating a "1" */
#define NEW_VALUE_MSG_TYPE  0

#define PRODUCER_INTERVAL   (1U * US_PER_SEC)   /* 1 second */
#define PRINT_INTERVAL      (10U * US_PER_SEC)  /* 10 seconds */

/* package constants into an enum type */
typedef enum {
    EVENT_1011 = 0,
    EVENT_0001,
} event_type_t; 

typedef struct {
    kernel_pid_t detectorA;
    kernel_pid_t detectorB;
} thread_pids_t;

/* a link of the linked list */
typedef struct {
    struct event_t *next;
    event_type_t type;
    char pattern[4];
} event_t;

char producer_stack[THREAD_STACKSIZE_DEFAULT]; 
static event_t *head = NULL;    /* head of the linked list */
static mutex_t list_lock;       /* mutex to prevent race conditions with the linked list */

void *producer_thread(void *arg)
{
    /* use input arg to extract the other thread process IDs */
    thread_pids_t *pids = (thread_pids_t *)arg; //cast it to the right type to parse

    msg_t msg;
    msg.type = NEW_VALUE_MSG_TYPE;
    double num;

    random_init(PRNG_INITVAL);

    while(1) {
        xtimer_usleep(PRODUCER_INTERVAL);
        printf("slept until...\n");
        num = random_real(); 
        if (num < PROB_OF_1) {
            /* a single ascii character is usually a byte, but we need to 
            upgrade it to a 32 bit unsigned integer to conform to the struct */
            msg.content.value = (uint32_t) '1';

            /* send a message to both detector threads */ 
            msg_send(&msg, pids->detectorA);
            msg_send(&msg, pids->detectorB);
        } else {
            /* a single ascii character is usually a byte, but we need to 
            upgrade it to a 32 bit unsigned integer to conform to the struct */
            msg.content.value = (uint32_t) '0';

            /* send a message to both detector threads */
            msg_send(&msg, pids->detectorA);
            msg_send(&msg, pids->detectorB);
        }
    }

    /* this should never be reached */
    return NULL;
}

void *detectorA_thread(void *arg) {
    msg_t msg;

    /* TODO: initialize a message queue of a size that is a power of 2 */

    while (1) {
        msg_receive(&msg)

        switch (msg.type) {
            case NEW_VALUE_MSG_TYPE:
                /* TODO: detect the pattern "1011" */
                
                /* TODO: if a full pattern is detected, add to the global*/
            default:
                DEBUG("dectectorA: unknown message type\n")
                break;

        }

    }

    /* this should never be reached */
    return NULL;
}

void *detectorB_thread(void *arg) {
    msg_t msg;

    /* TODO: initialize a message queue of a size that is a power of 2 */

    while (1) {
        msg_receive(&msg)

        switch (msg.type) {
            case NEW_VALUE_MSG_TYPE:
                /* TODO: detect the pattern "0001" */
                
                /* TODO: if a full pattern is detected, add */
            default:
                DEBUG("dectectorB: unknown message type\n")
                break;
        }
    }

    /* this should never be reached */
    return NULL;
}

int main(void)
{
    thread_pids_t pids; /* pass to producer_thread to minimize globals and hard coded values */

    /* TODO: create detectorA_thread and detectorB_thread (make sure to setup 
    their stacks) */

    //pids.detectorA = thread_create(???)
    
    //pids.detectorB = thread_create(???)

    thread_create(
            producer_stack, sizeof(producer_stack), // pass in stack
            THREAD_PRIORITY_MAIN - 1,               // set priority
            THREAD_CREATE_STACKTEST,                // for debugging
            producer_thread,                        // set thread function
            /* TODO: pass pids by reference and type cast to conform to function prototype */
            /* PASS_PIDS_HERE */,                   // thread func. input arg 
            "producer_thread");                     // thread name

    /* use main thread as the final printing thread */
    while (1) {
        xtimer_usleep(PRINT_INTERVAL);

        /* TODO: check if there are any events in the linked list and print the
        sequence strings comma-separated followed by the total number printed in 
        one iteration of this loop on a new line. make sure to use a mutex! */
    }

    /* this should never be reached */
    return 0;
}
