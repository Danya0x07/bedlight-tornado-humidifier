#ifndef INC_SENSORS_H
#define INC_SENSORS_H

typedef enum {
    WATER_ENOUGH,
    WATER_NOT_ENOUGH,
} waterlevel_t;

typedef struct {
    int16_t temperature;
    int16_t humidity;
    waterlevel_t waterlvl;
} sensordata_t;

void sensors_get_last_data(sensordata_t *data);
void *sensors_thd_cb(void *arg);

#endif // INC_SENSORS_H