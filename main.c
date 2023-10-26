#include <stdio.h>
#include <string.h>

#include "board.h"
#include "shell.h"
#include "shell_lock.h"
#include "led.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"
#include "soft_spi.h"
#include "dht.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#include "tornado.h"

extern void *thd_button(void *arg);
extern void *thd_dispatcher(void *arg);
extern void *thd_sensors(void *arg);
void *thd_tornadoctl(void *arg);

kernel_pid_t tornado_pid;

static int cb_generator(int argc, char **argv)
{
    if (argc == 2) {
        char *arg = argv[1];

        if (*arg == '1' || *arg == '0') {
            msg_send(&(msg_t){
                .type = TORNADO_GENERATOR_STATE, 
                .content.value = *arg - '0'
            }, tornado_pid);
        } 
        else {
            return -1;
        }
    }
    return 0;
}

static int cb_mist(int argc, char **argv)
{
    if (argc == 2) {
        char *arg = argv[1];

        if (*arg >= '0' && *arg <= '7') {
            msg_send(&(msg_t){
                .type = TORNADO_MIST_MODE, 
                .content.value = *arg - '0'
            }, tornado_pid);
        } 
        else {
            return -1;
        }
    }
    return 0;
}

static int cb_fan(int argc, char **argv)
{
    if (argc == 2) {
        char *arg = argv[1];

        if (*arg >= '0' && *arg <= '7') {
            msg_send(&(msg_t){
                .type = TORNADO_FAN_LEVEL, 
                .content.value = *arg - '0'
            }, tornado_pid);
        } 
        else {
            return -1;
        }
    }
    return 0;
}

static const shell_command_t shell_commands[] = {
    {"gen", "Set generator status", cb_generator},
    {"mist", "Set mist mode", cb_mist},
    {"fan", "Set fan level", cb_fan},
    {NULL, NULL, NULL}
};

static char thd01_stack[THREAD_STACKSIZE_MAIN];
static char thd02_stack[THREAD_STACKSIZE_MAIN];
static char thd03_stack[THREAD_STACKSIZE_MAIN];
static char thd04_stack[THREAD_STACKSIZE_MAIN];

extern kernel_pid_t dispatcher_pid;

int main(void)
{
    gpio_init(GPIO16, GPIO_IN);
    
    dispatcher_pid = thread_create(thd02_stack,
                  sizeof(thd02_stack),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_STACKTEST,
                  thd_dispatcher,
                  NULL,
                  "dispatcher thread");
    thread_create(thd01_stack,
                  sizeof(thd01_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  thd_button,
                  NULL,
                  "btn thread");
    thread_create(thd03_stack,
                  sizeof(thd03_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  thd_sensors,
                  NULL,
                  "sensors thread");
    
    tornado_pid = thread_create(thd04_stack,
                  sizeof(thd04_stack),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_STACKTEST,
                  thd_tornadoctl,
                  NULL,
                  "Tornado thread");
    

    printf("This application runs on %s\n", RIOT_BOARD);
    puts("This is Task-01");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    shell_lock_do_lock();

    return 0;
}
