#ifndef INC_DISPATCHER_H
#define INC_DISPATCHER_H

#include "thread.h"

typedef enum {
    EVENTSRC_BUTTON,
    EVENTSRC_SENSORS
} event_source_t;

extern kernel_pid_t dispatcher_pid;

void *dispatcher_thd_cb(void *arg);

#endif // INC_DISPATCHER_H