#include "msg.h"
#include "shell.h"
#include "shell_lock.h"

#include "console.h"
#include "tornado.h"
#include "sensors.h"

static int srd_cmd_cb(int argc, char **argv)
{
    sensordata_t data;

    sensors_get_last_data(&data);
    printf("Temp: %d, Hum: %d, Water: %d\n", data.temperature, data.humidity, data.waterlvl);
    return 0;
}

static int gen_cmd_cb(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }

    char *arg = argv[1];
    if (*arg == '1' || *arg == '0') {
        generator_state_t genstate = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_GENERATOR, .content.value=genstate}, tornado_pid);
    }
    return 0;
}

static int mist_cmd_cb(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }

    char *arg = argv[1];
    if (*arg >= '0' && *arg <= '7') {
        mist_mode_t mistmode = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_MIST, .content.value=mistmode}, tornado_pid);
    } 
    return 0;
}

static int fan_cmd_cb(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }

    char *arg = argv[1];
    if (*arg >= '0' && *arg <= '7') {
        fan_level_t fanlvl = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_FAN, .content.value=fanlvl}, tornado_pid);
    } 
    return 0;
}

static const shell_command_t shell_commands[] = {
    {"srd", "Show last data from sensors", srd_cmd_cb},
    {"gen", "Set generator state (1/0)", gen_cmd_cb},
    {"mist", "Set mist mode (0..7)", mist_cmd_cb},
    {"fan", "Set fan level (0..7)", fan_cmd_cb},
    {NULL, NULL, NULL}
};

void console_run(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}