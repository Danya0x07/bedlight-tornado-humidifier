#include "periph/gpio.h"
#include "xtimer.h"
#include "button.h"

bool button_read(void)
{
    return gpio_read(BUTTON_PIN);
}

button_ev_t button_read_event(void)
{
    static bool prev_state = false;
    bool state = button_read();
    button_ev_t event = BTNEV_NOTHING;

    if (state != prev_state) {
        xtimer_msleep(5);
        state = button_read();
    }

    if (state && !prev_state)
        event = BTNEV_PRESS;
    else if (!state && prev_state)
        event = BTNEV_RELEASE;
    
    prev_state = state;
    return event;
}

button_ev_t button_another_press(void)
{
    button_ev_t event = BTNEV_NOTHING;
    
    for (int i = 0; i < 200; i++) {
        xtimer_msleep(1);
        if (event == BTNEV_NOTHING)
            event = button_read_event();
        else if (event == BTNEV_RELEASE && button_read_event() == BTNEV_PRESS) {
            event = BTNEV_PRESS;
            break;
        }
    }
    return event;
}