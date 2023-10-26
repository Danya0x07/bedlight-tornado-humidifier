#ifndef _BOARD_H
#define _BOARD_H

#include <stdbool.h>

#ifdef CPU_ESP8266
#   define SOFT_SPI_PARAM_MOSI  GPIO14
#   define SOFT_SPI_PARAM_CLK   GPIO13
#   define BUTTON_PIN   GPIO16
#   define SHREG_CS_PIN GPIO12
#endif

#define WATER_ADC_THRESHOLD  512

#include_next "board.h"

#endif