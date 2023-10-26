#define ENABLE_DEBUG    1
#include "debug.h"
#include "thread.h"
#include "xtimer.h"
#include "soft_spi.h"
#include "periph/adc.h"
#include "msg.h"
#include "dht.h"

#include "button.h"
#include "tornado.h"

typedef enum {
    EVTYPE_BTNEVENT,
    EVTYPE_SENSORDATA
} event_type_t;



kernel_pid_t dispatcher_pid;

void *thd_button(void *arg)
{
    (void) arg;
    button_ev_t event;
    uint32_t timestamp;
    bool maybe_longpress = false;
    msg_t msg = {.type = EVTYPE_BTNEVENT};

    for (;;) {
        event = button_read_event();
        if (event == BTNEV_PRESS) {
            button_ev_t next_ev = button_another_press();
            if (next_ev == BTNEV_PRESS) {
                if (button_another_press() == BTNEV_PRESS) {
                    msg.content.value = 3;
                } 
                else {
                    msg.content.value = 2;
                }
            }
            else if (next_ev == BTNEV_RELEASE) {
                msg.content.value = 1;
            }
            else {
                maybe_longpress = true;
                timestamp = xtimer_now_usec();
            }
        }
        else if (event == BTNEV_RELEASE) {
            if (maybe_longpress) {
                if (xtimer_now_usec() - timestamp >= 3000000) {
                    msg.content.value = 4;
                }
                maybe_longpress = false;
            }
        }
        if (msg.content.value != 0) {
            msg_send(&msg, dispatcher_pid);
            msg.content.value = 0;
        }
        xtimer_msleep(100);
    }
}

typedef enum __packed {
    WATER_ENOUGH,
    WATER_NOT_ENOUGH,
} waterlevel_t;

waterlevel_t calc_water_level(int16_t adcvalue)
{
    return adcvalue > 512 ? WATER_ENOUGH : WATER_NOT_ENOUGH;
}

typedef struct {
    int16_t temperature;
    int16_t humidity;
    waterlevel_t waterlvl;
} sensordata_t;



void *thd_sensors(void *arg)
{
    (void) arg;

    dht_t dht22;
    dht_params_t dht_params = {.type=DHT22, .pin=GPIO4, .in_mode=GPIO_IN};
    
    sensordata_t sensordata;
    msg_t msg = {.type = EVTYPE_SENSORDATA, .content.ptr = &sensordata};

    dht_init(&dht22, &dht_params);
    adc_init(0);

    for (;;) {
        dht_read(&dht22, &sensordata.temperature, &sensordata.humidity);
        sensordata.waterlvl = calc_water_level(adc_sample(0, ADC_RES_10BIT));
        msg_send(&msg, dispatcher_pid);
        xtimer_sleep(10);
    }
}

void *thd_console(void *arg)
{
    (void) arg;

    for (;;) {
        
    }
}

static msg_t tornadoctl_queue[8];

void *thd_tornadoctl(void *arg)
{
    (void) arg;
    msg_t msg;
    msg_init_queue(tornadoctl_queue, sizeof(tornadoctl_queue) / sizeof(tornadoctl_queue[0]));
    
    soft_spi_init(0);
    soft_spi_init_cs(0, SHREG_CS_PIN);
    soft_spi_acquire(0, SHREG_CS_PIN, SOFT_SPI_MODE_0, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(0, SHREG_CS_PIN, false, 0);

    uint8_t ctlbyte = 0, prev_ctlbyte = 0;

    generator_state_t genstate = GENERATOR_OFF;
    mist_mode_t mistmode = MIST_OFF;
    fan_level_t fanlvl = FAN_OFF;

    for (;;) {
        msg_receive(&msg);
        switch (msg.type) {
            case TORNADO_GENERATOR_STATE:
                genstate = (generator_state_t)(msg.content.value & 1);
                if (genstate == GENERATOR_OFF) {
                    mistmode = MIST_OFF;
                }
                break;
            
            case TORNADO_MIST_MODE:
                mistmode = (mist_mode_t)(msg.content.value & 0b111);
                if (mistmode != MIST_OFF) {
                    genstate = GENERATOR_ON;
                    if (fanlvl == FAN_OFF) {
                        fanlvl = FAN_LVL1;
                    }
                }
                break;
            
            case TORNADO_FAN_LEVEL:
                fanlvl = (fan_level_t)(msg.content.value & 0b111);
                if (fanlvl == FAN_OFF) {
                    mistmode = MIST_OFF;
                }
                break;
        }
        ctlbyte = (uint8_t)(fanlvl << 5 | genstate << 4 | mistmode << 1); 
        if (ctlbyte != prev_ctlbyte) {
            soft_spi_transfer_byte(0, SHREG_CS_PIN, false, ctlbyte);
            prev_ctlbyte = ctlbyte;
        }
    }
}

void *thd_ledstrip(void *arg)
{
    (void) arg;

    for (;;) {
        
    }
}

static msg_t dispatcher_queue[32];

void *thd_dispatcher(void *arg)
{
    (void) arg;
    msg_t msg;
    msg_init_queue(dispatcher_queue, sizeof(dispatcher_queue) / sizeof(dispatcher_queue[0]));
    sensordata_t sensordata;

    for (;;) {
        msg_receive(&msg);
        switch (msg.type) {
            case EVTYPE_BTNEVENT:
                printf("Button event: %d\n", msg.content.value);
                break;
            
            case EVTYPE_SENSORDATA:
                sensordata = *((sensordata_t *)msg.content.ptr);
                printf("Temp: %d, Hum: %d, Water: %d\n", sensordata.temperature, sensordata.humidity, sensordata.waterlvl);
                break;
        }
    }
}

void *thd_network(void *arg)
{
    (void) arg;

    for (;;) {
        
    }
}