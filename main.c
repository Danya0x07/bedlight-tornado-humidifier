#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "led.h"
#include "thread.h"
#include "xtimer.h"
#include "msg.h"
#include "soft_spi.h"
#include "dht.h"
#include "periph/adc.h"
#include "periph/gpio.h"

void *thd_my01_cb(void *arg)
{
    (void) arg;
    soft_spi_init(0);
    soft_spi_cs_t shreg_cs = GPIO12;

    if (soft_spi_init_cs(0, shreg_cs) < 0) {
        LED0_ON;
    }

    dht_t dht22;
    dht_params_t dht_params = {.type=DHT22, .pin=GPIO4, .in_mode=GPIO_IN};
    dht_init(&dht22, &dht_params);

    adc_init(0);
    
    soft_spi_acquire(0, shreg_cs, SOFT_SPI_MODE_0, SOFT_SPI_CLK_100KHZ);
    int16_t temp=0, hum=0;
    for (uint8_t i = 0;; i++) {
        soft_spi_transfer_byte(0, shreg_cs, false, i << 1);
        dht_read(&dht22, &temp, &hum);
        printf("Temp: %d, Hum: %d, ADC: %d, btn: %d\n", temp, hum, adc_sample(0, ADC_RES_10BIT), gpio_read(GPIO16));
        
        xtimer_msleep(1000);
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
    gpio_init(GPIO16, GPIO_IN);

    thread_create(thd01_stack,
                  sizeof(thd01_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  thd_my01_cb,
                  NULL,
                  "my01 thread");

    printf("This application runs on %s\n", RIOT_BOARD);
    puts("This is Task-01");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
