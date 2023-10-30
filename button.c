#include "periph/gpio.h"
#include "xtimer.h"
#include "log_module.h"

#include "button.h"
#include "board.h"
#include "dispatcher.h"

typedef enum {
    NOTHING,
    PRESS,
    RELEASE
} button_ll_event_t;

static bool btn_read_state(void)
{
    return gpio_read(BUTTON_PIN);
}

static button_ll_event_t btn_read_event(void)
{
    static bool prev_state = false;
    bool state = btn_read_state();
    button_ll_event_t event = NOTHING;

#ifdef DEMOBOARD // Debounce button
    if (state != prev_state) {
        xtimer_msleep(5);
        state = btn_read_state();
    }
#endif

    if (state && !prev_state)
        event = PRESS;
    else if (!state && prev_state)
        event = RELEASE;

    prev_state = state;
    return event;
}

static button_ll_event_t btn_read_next_event(void)
{
    button_ll_event_t event = NOTHING;

    for (int i = 0; i < 200; i++) {
        xtimer_msleep(1);
        if (event == NOTHING)
            event = btn_read_event();
        else if (event == RELEASE && btn_read_event() == PRESS) {
            event = PRESS;
            break;
        }
    }
    return event;
}

void *button_thd_cb(void *arg)
{
    (void) arg;
    button_ll_event_t ll_event;
    button_event_t event;
    uint32_t timestamp;
    bool maybe_longpress = false;

    gpio_init(BUTTON_PIN, GPIO_IN);

    log_write(LOG_INFO, "Button thread started\n");
    for (;;) {
        event = BTN_EV_NONE;
        ll_event = btn_read_event();

        if (ll_event == PRESS) {
            button_ll_event_t next_ev = btn_read_next_event();
            if (next_ev == PRESS) {
                if (btn_read_next_event() == PRESS) {
                    event = BTN_EV_3PRESS;
                }
                else {
                    event = BTN_EV_2PRESS;
                }
            }
            else if (next_ev == RELEASE) {
                event = BTN_EV_PRESS;
            }
            else {
                maybe_longpress = true;
                timestamp = xtimer_now_usec();
            }
        }
        else if (ll_event == RELEASE) {
            if (maybe_longpress) {
                if (xtimer_now_usec() - timestamp >= 3000000) {
                    event = BTN_EV_LPRESS;
                }
                maybe_longpress = false;
            }
        }
        if (event != BTN_EV_NONE) {
            msg_send(&(msg_t){.type=EVENTSRC_BUTTON, .content.value=event}, dispatcher_pid);
        }
        xtimer_msleep(100);
    }
}