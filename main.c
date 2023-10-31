#include "thread.h"
#include "mutex.h"
#include "ztimer.h"
#include "vfs.h"
#include "mtd.h"
#include "fs/spiffs_fs.h"
#include "log_module.h"
#include "tiny_strerror.h"

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
static char tornado_thd_stack[THREAD_STACKSIZE_LARGE];

static void create_threads(void)
{
    dispatcher_pid = thread_create(dispatcher_thd_stack,
                                   sizeof(dispatcher_thd_stack),
                                   THREAD_PRIORITY_MAIN + 3,
                                   THREAD_CREATE_STACKTEST | THREAD_CREATE_WOUT_YIELD,
                                   dispatcher_thd_cb,
                                   NULL,
                                   "dispatcher");
    if (dispatcher_pid < 0) {
        log_write(LOG_ERROR, "Unable to create dispatcher thread: error %s\n", strerror(dispatcher_pid));
    }

    button_pid = thread_create(button_thd_stack,
                               sizeof(button_thd_stack),
                               THREAD_PRIORITY_MAIN + 1,
                               THREAD_CREATE_STACKTEST  | THREAD_CREATE_WOUT_YIELD,
                               button_thd_cb,
                               NULL,
                               "button");
    if (button_pid < 0) {
        log_write(LOG_ERROR, "Unable to create button thread: error %s\n", strerror(button_pid));
    }

    sensors_pid = thread_create(sensors_thd_stack,
                                sizeof(sensors_thd_stack),
                                THREAD_PRIORITY_MAIN + 2,
                                THREAD_CREATE_STACKTEST  | THREAD_CREATE_WOUT_YIELD,
                                sensors_thd_cb,
                                NULL,
                                "sensors");
    if (sensors_pid < 0) {
        log_write(LOG_ERROR, "Unable to create sensors thread: error %s\n", strerror(sensors_pid));
    }
    
    tornado_pid = thread_create(tornado_thd_stack,
                                sizeof(tornado_thd_stack),
                                THREAD_PRIORITY_MAIN + 4,
                                THREAD_CREATE_STACKTEST  | THREAD_CREATE_WOUT_YIELD,
                                tornadoctl_thd_cb,
                                NULL,
                                "tornado");
    if (tornado_pid < 0) {
        log_write(LOG_ERROR, "Unable to create tornado thread: error %s\n", strerror(tornado_pid));
    }
}

static void init_filesystem(void)
{
    static struct spiffs_desc spiffs_desc = {.lock = MUTEX_INIT};
    static vfs_mount_t spiffs_mount = {
        .fs = &spiffs_file_system,
        .mount_point = "/spiffs",
        .private_data = &spiffs_desc
    };
    spiffs_desc.dev = MTD_0;

    int res;

    if ((res = mtd_init(MTD_0)) < 0) {
        log_write(LOG_ERROR, "Error initializing MTD: %s\n", strerror(res));
        return;
    }
    
    if ((res = vfs_mount(&spiffs_mount)) < 0) {
        log_write(LOG_WARNING, "Cannot mount SPIFFS, formating...\n");

        if ((res = vfs_format(&spiffs_mount)) < 0) {
            log_write(LOG_ERROR, "Error formating SPIFFS: %s\n", strerror(res));
            return;
        }

        if ((res = vfs_mount(&spiffs_mount)) < 0) {
            log_write(LOG_ERROR, "Error mounting SPIFFS: %s\n", strerror(res));
        }
    }
}

int main(void)
{
    init_filesystem();
    create_threads();
    
    //thread_wakeup(dispatcher_pid);
    //thread_wakeup(tornado_pid);
    //thread_wakeup(sensors_pid);
    //thread_wakeup(button_pid);

    ztimer_sleep(ZTIMER_SEC, 2);

    console_run();

    return 0;
}
