#ifndef INC_BUTTON_H
#define INC_BUTTON_H

#include "periph/gpio.h"

typedef enum {
    BTNEV_NOTHING,
    BTNEV_PRESS,
    BTNEV_RELEASE
} button_ev_t;

bool button_read(void);
button_ev_t button_read_event(void);
button_ev_t button_another_press(void);

#endif // INC_BUTTON_H