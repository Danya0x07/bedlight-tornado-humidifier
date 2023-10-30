#include "msg.h"
#include "log_module.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#include "dispatcher.h"
#include "button.h"
#include "sensors.h"
#include "tornado.h"

static msg_t msg_queue[32];

void *dispatcher_thd_cb(void *arg)
{
    (void) arg;
    msg_t msg;
    msg_init_queue(msg_queue, sizeof(msg_queue) / sizeof(msg_queue[0]));

    button_event_t button_event;
    sensordata_t *sensordata;

    log_write(LOG_INFO, "Dispatcher thread started\n");
    for (;;) {
        msg_receive(&msg);
        switch (msg.type) {
            case EVENTSRC_BUTTON:
                button_event = msg.content.value;
                DEBUG("Button event: %d\n", button_event);
                break;
            
            case EVENTSRC_SENSORS:
                sensordata = msg.content.ptr;
                (void) sensordata;
                break;
        }
    }
}