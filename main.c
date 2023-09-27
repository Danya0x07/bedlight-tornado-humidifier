#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "led.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"

#define Q_SZ    32

void *thd_my01_cb(void *arg)
{
    (void) arg;
    for (;;) {
        //LED0_TOGGLE;
        xtimer_msleep(500);
    }

    return NULL;
}

static int cb_fly(int argc, char **argv)
{
    for (int i = 0; i < argc; i++) {
        printf("Arg: %s\n", argv[i]);
    }
    return argc == 5;
}

static const shell_command_t shell_commands[] = {
    {"fly", "Fly your wings", cb_fly},
    {NULL, NULL, NULL}
};

static char thd01_stack[THREAD_STACKSIZE_MAIN];

int main(void)
{
    //thread_create(thd01_stack,
                  //sizeof(thd01_stack),
                  //THREAD_PRIORITY_MAIN - 1,
                  //THREAD_CREATE_STACKTEST,
                  //thd_my01_cb,
                  //NULL,
                  //"my01 thread");

    thread_create(thd02_stack,
                  sizeof(thd02_stack),
                  THREAD_PRIORITY_MAIN,
                  THREAD_CREATE_STACKTEST,
                  _event_loop,
                  NULL,
                  "my02 thread");

    printf("This application runs on %s\n", RIOT_BOARD);
    puts("This is Task-01");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);



    return 0;
}
