#include "periph/adc.h"
#include "ztimer.h"
#include "mutex.h"
#include "msg.h"
#include "dht.h"
#include "log_module.h"

#define ENABLE_DEBUG    1
#include "debug.h"

#include "sensors.h"
#include "board.h"
#include "dispatcher.h"

#define WATER_ADC_THRESHOLD  512  // TODO: calibrate

static sensordata_t sensordata = {0};
static mutex_t sensordata_mx = MUTEX_INIT;

static waterlevel_t calc_water_level(int16_t adcvalue)
{
    return adcvalue > WATER_ADC_THRESHOLD ? WATER_ENOUGH : WATER_NOT_ENOUGH;
}

void sensors_get_last_data(sensordata_t *data)
{
    mutex_lock(&sensordata_mx);
    *data = sensordata;
    mutex_unlock(&sensordata_mx);
}

void *sensors_thd_cb(void *arg)
{
    (void) arg;

    dht_t dht22;
    dht_params_t dht_params = {.type=DHT22, .pin=DHT_PIN, .in_mode=GPIO_IN};
    int ret;

    adc_init(0);

    if ((ret = dht_init(&dht22, &dht_params)) < 0) {
        log_write(LOG_ERROR, "DHT initialization error: %d\n", ret);
    }

    msg_t msg = {.type = EVENTSRC_SENSORS};
    int32_t sample;

    log_write(LOG_INFO, "Sensors thread started\n");
    for (;;) {
        mutex_lock(&sensordata_mx);

        sample = adc_sample(0, ADC_RES_10BIT);
        DEBUG("ADC sample: %d\n", sample);
        sensordata.waterlvl = calc_water_level(sample);

        ret = dht_read(&dht22, &sensordata.temperature, &sensordata.humidity);
        if (ret < 0) {
            log_write(LOG_ERROR, "DHT reading error: %d\n", ret);
        }

        mutex_unlock(&sensordata_mx);

        msg_send(&msg, dispatcher_pid);

        ztimer_sleep(ZTIMER_SEC, 10);
    }
}