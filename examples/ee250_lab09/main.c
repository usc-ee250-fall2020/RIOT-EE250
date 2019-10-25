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
#include <stdlib.h>
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

#define PRODUCER_INTERVAL   (50000)   /* 1 second */
#define PRINT_INTERVAL      (2 * US_PER_SEC)  /* 10 seconds */

#define PRNG_INITVAL            418 /* seed value for pseudorandom number generator */
#define PROB_OF_1_IN_PERCENT    30 /* probability of generating a "1" */
#define NEW_VALUE_MSG_TYPE      0
#define PATTERN_LEN             4

typedef struct {
    kernel_pid_t detectorA;
    kernel_pid_t detectorB;
} thread_pids_t;

/* a link of the linked list */
typedef struct event {
    struct event *next;
    char pattern[PATTERN_LEN];
} event_t;

char producer_stack[THREAD_STACKSIZE_DEFAULT]; 
/* TODO: setup thread stacks */

/* TODO: setup msg queues for any threads that need it */


static event_t *head = NULL;    /* head of the linked list */
static mutex_t list_lock;       /* mutex to prevent race conditions with the 
                                   linked list */

void *producer_thread(void *arg)
{
    /* use input arg to extract the other thread process IDs */
    thread_pids_t *pids = (thread_pids_t *)arg; /* cast it to the right type to parse */

    msg_t msg;
    msg.type = NEW_VALUE_MSG_TYPE;
    uint32_t num;

    random_init(PRNG_INITVAL);

    while(1) {
        xtimer_usleep(PRODUCER_INTERVAL);
        num = random_uint32_range(0, 100);
        if (num < PROB_OF_1_IN_PERCENT) {
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
    /* TODO: remove compiler warning of unused `arg` */

    msg_t msg;
    int state = 0;
    char c;

    /* TODO: initialize a message queue of a size that is a power of 2 */

    while (1) {
        /* TODO: use a blocking call to receive a msg_t. see msg.h for help */

        switch (msg.type) {
            case NEW_VALUE_MSG_TYPE:
                /* TODO: extract the content out of the message. you should 
                   typecast it accordingly upon extraction! */


                DEBUG("detectorA: new incoming character %c!\n", c);

                /* TODO: detect the pattern "1011" using a state machine */
                switch (state) {
                    /* TODO: when a pattern is detected, allocate memory for a
                       new event_t and append it to the linked list */

                    /* Tip: it may be helpful to have two pointers to an 
                       event_t, one for holding the new link and one to iterate
                       through the linked list. use DEBUG() to print events
                       for debugging so you can turn them off before demos. */

                } /* switch */
                break;
                
            default:
                DEBUG("dectectorA: unknown message type\n");
                break;
        }
    }

    /* this should never be reached */
    return NULL;
}

void *detectorB_thread(void *arg) {
    /* TODO: remove compiler warning of unused `arg` */

    msg_t msg;
    int state = 0;
    char c;

    /* TODO: initialize a message queue of a size that is a power of 2 */

    while (1) {
        /* TODO: use a blocking call to receive a msg_t. see msg.h for help */

        switch (msg.type) {
            case NEW_VALUE_MSG_TYPE:
                /* TODO: extract the content out of the message. you should 
                   typecast it accordingly upon extraction! */


                DEBUG("detectorB: new incoming character %c!\n", c);

                /* TODO: detect the pattern "0001" using a state machine */
                switch (state) {
                    /* TODO: when a pattern is detected, allocate memory for a
                       new event_t and append it to the linked list */

                    /* Tip: it may be helpful to have two pointers to an 
                       event_t, one for holding the new link and one to iterate
                       through the linked list. use DEBUG() to print events
                       for debugging so you can turn them off before demos. */

                } /* switch */
                break;
                
            default:
                DEBUG("dectectorB: unknown message type\n");
                break;
        }
    }

    /* this should never be reached */
    return NULL;
}

int main(void)
{
    thread_pids_t pids; /* pass to producer_thread to minimize globals and hard coded values */
    event_t *event;
    int count = 0;

    /* initialize list_lock (see mutex.h) */

    /* TODO: create detectorA_thread and detectorB_thread. make sure to setup 
       their stacks and pass NULL for the arg since we will not use it. */

    thread_create(
            producer_stack, sizeof(producer_stack), // pass in stack
            THREAD_PRIORITY_MAIN - 1,               // set priority
            THREAD_CREATE_STACKTEST,                // for debugging
            producer_thread,                        // set thread function
            /* TODO: pass pids by reference and type cast to conform to the 
               thread_create function prototype */
            /* pass pids here*/,                    // thread func. input arg 
            "producer_thread");                     // thread name

    /* use main thread as the final printing thread */
    while (1) {
        xtimer_usleep(PRINT_INTERVAL);

        /* TODO: check if there are any events in the linked list and print the
        sequence strings comma-separated followed by the total number printed in 
        one iteration of this loop on a new line. make sure to use a mutex! */

        /* Tips: 
            - you will need to use the poitner "event" above as a temp variable
            - use printf() instead of DEBUG() because this will be in the final
              printouts
         */

        printf("TOTAL_COUNT = %d\n", count);
        count = 0;
    }

    /* this should never be reached */
    return 0;
}
