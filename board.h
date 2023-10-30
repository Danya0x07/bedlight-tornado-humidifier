#ifndef _BOARD_H
#define _BOARD_H

#ifdef CPU_ESP8266
#   define SOFT_SPI_PARAM_MOSI  GPIO14
#   define SOFT_SPI_PARAM_CLK   GPIO13
#   define BUTTON_PIN   GPIO16
#   define SHREG_CS_PIN GPIO12
#   define DHT_PIN  GPIO4
#endif

#include_next "board.h"

#endif