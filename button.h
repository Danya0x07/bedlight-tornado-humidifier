#ifndef INC_BUTTON_H
#define INC_BUTTON_H

typedef enum {
    BTN_EV_NONE,
    BTN_EV_PRESS,
    BTN_EV_2PRESS,
    BTN_EV_3PRESS,
    BTN_EV_LPRESS
} button_event_t;

void *button_thd_cb(void *arg);

#endif // INC_BUTTON_H