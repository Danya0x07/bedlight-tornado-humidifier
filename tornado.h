#ifndef INC_TORNADO_H
#define INC_TORNADO_H

#include "thread.h"

typedef enum {
    TORNADO_CTLRQ_GENERATOR,
    TORNADO_CTLRQ_MIST,
    TORNADO_CTLRQ_FAN
} tornado_control_request_t;

typedef enum {
    GENERATOR_OFF,
    GENERATOR_ON
} generator_state_t;

typedef enum {
    MIST_OFF = 0b000,
    MIST_LOW1 = 0b001,
    MIST_LOW2 = 0b010,
    MIST_LOW3 = 0b100,
    MIST_MEDIUM1 = 0b011,
    MIST_MEDIUM2 = 0b101,
    MIST_MEDIUM3 = 0b110,
    MIST_FULL = 0b111
} mist_mode_t;

typedef enum {
    FAN_OFF = 0b000,
    FAN_LVL1 = 0b001, // 62 Ohm
    FAN_LVL2 = 0b010, // 43 Ohm
    FAN_LVL3 = 0b100, // 27 Ohm
    FAN_LVL4 = 0b011, // 62||43 ≈ 25 Ohm
    FAN_LVL5 = 0b101, // 62||27 ≈ 18 Ohm
    FAN_LVL6 = 0b110, // 43||27 ≈ 17 Ohm
    FAN_LVL7 = 0b111, // 62||43||27 ≈ 13 Ohm
} fan_level_t;

extern kernel_pid_t tornado_pid;

void *tornadoctl_thd_cb(void *arg);

#endif // INC_TORNADO_H