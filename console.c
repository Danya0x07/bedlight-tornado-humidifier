#include <stdio.h>
#include <unistd.h>
#include "msg.h"
#include "shell.h"
#include "shell_lock.h"

#include "console.h"
#include "tornado.h"
#include "sensors.h"

static int _srd(int argc, char **argv)
{
    sensordata_t data;

    sensors_get_last_data(&data);
    printf("Temp: %d, Hum: %d, Water: %d\n", data.temperature, data.humidity, data.waterlvl);
    return 0;
}

static int _gen(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <state: 1|0>", argv[0]);
        return 1;
    }

    char *arg = argv[1];
    if (*arg == '1' || *arg == '0') {
        generator_state_t genstate = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_GENERATOR, .content.value=genstate}, tornado_pid);
    }
    return 0;
}

static int _mist(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <mode: 0..7>\n", argv[0]);
        return 1;
    }

    char *arg = argv[1];
    if (*arg >= '0' && *arg <= '7') {
        mist_mode_t mistmode = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_MIST, .content.value=mistmode}, tornado_pid);
    } 
    return 0;
}

static int _fan(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <level: 0..7>\n", argv[0]);
        return 1;
    }

    char *arg = argv[1];
    if (*arg >= '0' && *arg <= '7') {
        fan_level_t fanlvl = *arg - '0';
        msg_send(&(msg_t){.type=TORNADO_CTLRQ_FAN, .content.value=fanlvl}, tornado_pid);
    } 
    return 0;
}

static int _cat(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("file %s does not exist\n", argv[1]);
        return 1;
    }

    char c;
    while (fread(&c, 1, 1, f) != 0) {
        putchar(c);
    }
    putchar('\n'); // Otherwise will print only on next newline.
    fclose(f);

    flush(stdout);
    return 0;
}

static int _tee(int argc, char **argv)
{
    if (argc != 3) {
        printf("Usage: %s <file> <str>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "w+");
    if (f == NULL) {
        printf("error while trying to create %s\n", argv[1]);
        return 1;
    }

    if (fwrite(argv[2], 1, strlen(argv[2]), f) != strlen(argv[2])) {
        puts("Error while writing");
    }
    fclose(f);

    return 0;
}

static const shell_command_t shell_commands[] = {
    {"srd", "Show last data from sensors", _srd},
    {"gen", "Set generator state (1/0)", _gen},
    {"mist", "Set mist mode (0..7)", _mist},
    {"fan", "Set fan level (0..7)", _fan},
    {"cat", "print the content of a file", _cat},
    {"tee", "write a string in a file", _tee},
    {NULL, NULL, NULL}
};

void console_run(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}