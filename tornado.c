#include "msg.h"
#include "soft_spi.h"
#include "log_module.h"

#include "tornado.h"
#include "board.h"

static msg_t msg_queue[8];

void *tornadoctl_thd_cb(void *arg)
{
    (void) arg;
    
    soft_spi_init(0);
    soft_spi_init_cs(0, SHREG_CS_PIN);
    soft_spi_acquire(0, SHREG_CS_PIN, SOFT_SPI_MODE_0, SOFT_SPI_CLK_100KHZ);
    soft_spi_transfer_byte(0, SHREG_CS_PIN, false, 0);

    generator_state_t genstate = GENERATOR_OFF;
    mist_mode_t mistmode = MIST_OFF;
    fan_level_t fanlvl = FAN_OFF;
    uint8_t ctlbyte = 0, prev_ctlbyte = 0;

    msg_t msg;
    msg_init_queue(msg_queue, sizeof(msg_queue) / sizeof(msg_queue[0]));

    log_write(LOG_INFO, "Tornado thread started\n");
    for (;;) {
        msg_receive(&msg);
        switch (msg.type) {
            case TORNADO_CTLRQ_GENERATOR:
                genstate = (generator_state_t)(msg.content.value & 1);
                if (genstate == GENERATOR_OFF) {
                    log_write(LOG_INFO, "Generator off.\n");
                    if (mistmode != MIST_OFF) {
                        log_write(LOG_WARNING, "Mist off.\n");
                        mistmode = MIST_OFF;
                    }
                }
                else {
                    log_write(LOG_INFO, "Generator on.\n");
                }
                break;
            
            case TORNADO_CTLRQ_MIST:
                mistmode = (mist_mode_t)(msg.content.value & 0b111);
                if (mistmode != MIST_OFF) {
                    log_write(LOG_INFO, "Mist mode: %d\n", mistmode);
                    if (genstate == GENERATOR_OFF) {
                        log_write(LOG_WARNING, "Generator on.\n");
                        genstate = GENERATOR_ON;
                    }
                    
                    if (fanlvl == FAN_OFF) {
                        log_write(LOG_WARNING, "Fan on.\n");
                        fanlvl = FAN_LVL1;
                    }
                }
                else {
                    log_write(LOG_INFO, "Mist off.\n");
                }
                break;
            
            case TORNADO_CTLRQ_FAN:
                fanlvl = (fan_level_t)(msg.content.value & 0b111);
                if (fanlvl == FAN_OFF) {
                    log_write(LOG_INFO, "Fan off.\n");
                    if (mistmode != MIST_OFF) {
                        log_write(LOG_WARNING, "Mist off.\n");
                        mistmode = MIST_OFF;
                    }
                }
                else {
                    log_write(LOG_INFO, "Fan level: %d.\n", fanlvl);
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