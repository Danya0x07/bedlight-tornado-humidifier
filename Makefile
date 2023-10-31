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

INCLUDES += -I$(APPDIR)

USEMODULE += ztimer ztimer_sec ztimer_msec
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_lock
USEMODULE += ps
USEMODULE += log_color

USEMODULE += soft_spi
USEMODULE += periph_adc
USEMODULE += dht
USEMODULE += esp_spiffs
USEMODULE += tiny_strerror_as_strerror
USEMODULE += esp_log_startup

CFLAGS += -DCONFIG_SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS="3 * 60 * 1000"
CFLAGS += -DMODULE_SHELL_LOCK_AUTO_LOCKING
CFLAGS += -DDEMOBOARD
CFLAGS += -DCONFIG_USE_HARDWARE_MTD

include ./Makefile.prv
include $(RIOTBASE)/Makefile.include
