#include "thread.h"
#include "ztimer.h"
#include "log_module.h"

#include "dispatcher.h"
#include "button.h"
#include "sensors.h"
#include "tornado.h"
#include "console.h"

kernel_pid_t dispatcher_pid;
kernel_pid_t tornado_pid;
static kernel_pid_t button_pid;
static kernel_pid_t sensors_pid;

static char dispatcher_thd_stack[THREAD_STACKSIZE_DEFAULT];
static char button_thd_stack[THREAD_STACKSIZE_DEFAULT];
static char sensors_thd_stack[THREAD_STACKSIZE_DEFAULT];
static char tornado_thd_stack[THREAD_STACKSIZE_DEFAULT];

int main(void)
{
    
    dispatcher_pid = thread_create(dispatcher_thd_stack,
                                   sizeof(dispatcher_thd_stack),
                                   THREAD_PRIORITY_MAIN + 3,
                                   THREAD_CREATE_STACKTEST | THREAD_CREATE_SLEEPING,
                                   dispatcher_thd_cb,
                                   NULL,
                                   "dispatcher");
    if (dispatcher_pid < 0) {
        log_write(LOG_ERROR, "Unable to create dispatcher thread: error %d\n", dispatcher_pid);
    }

    button_pid = thread_create(button_thd_stack,
                               sizeof(button_thd_stack),
                               THREAD_PRIORITY_MAIN + 1,
                               THREAD_CREATE_STACKTEST  | THREAD_CREATE_SLEEPING,
                               button_thd_cb,
                               NULL,
                               "button");
    if (button_pid < 0) {
        log_write(LOG_ERROR, "Unable to create button thread: error %d\n", button_pid);
    }

    sensors_pid = thread_create(sensors_thd_stack,
                                sizeof(sensors_thd_stack),
                                THREAD_PRIORITY_MAIN + 2,
                                THREAD_CREATE_STACKTEST  | THREAD_CREATE_SLEEPING,
                                sensors_thd_cb,
                                NULL,
                                "sensors");
    if (sensors_pid < 0) {
        log_write(LOG_ERROR, "Unable to create sensors thread: error %d\n", sensors_pid);
    }
    
    tornado_pid = thread_create(tornado_thd_stack,
                                sizeof(tornado_thd_stack),
                                THREAD_PRIORITY_MAIN + 4,
                                THREAD_CREATE_STACKTEST  | THREAD_CREATE_SLEEPING,
                                tornadoctl_thd_cb,
                                NULL,
                                "tornado");
    if (tornado_pid < 0) {
        log_write(LOG_ERROR, "Unable to create tornado thread: error %d\n", tornado_pid);
    }
    
    thread_wakeup(dispatcher_pid);
    thread_wakeup(tornado_pid);
    thread_wakeup(sensors_pid);
    thread_wakeup(button_pid);

    ztimer_sleep(ZTIMER_SEC, 3);

    console_run();

    return 0;
}
