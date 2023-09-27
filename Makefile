# name of your application
APPLICATION = bedlight_tornado_humidifier

# If no BOARD is found in the environment, use this default:
BOARD ?= esp8266-esp-12x

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../Templates/RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += xtimer

USEMODULE += soft_spi
CFLAGS += -DSOFT_SPI_PARAM_MOSI=GPIO14 -DSOFT_SPI_PARAM_CLK=GPIO13
USEMODULE += periph_adc

USEMODULE += dht

include $(RIOTBASE)/Makefile.include
