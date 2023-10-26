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

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += shell_lock
USEMODULE += ps
USEMODULE += xtimer

USEMODULE += soft_spi
USEMODULE += periph_adc

USEMODULE += dht

CFLAGS += -DCONFIG_SHELL_LOCK_PASSWORD=\"asdfg\" 
CFLAGS += -DCONFIG_SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS="1 * 60 * 1000"
CFLAGS += -DMODULE_SHELL_LOCK_AUTO_LOCKING

include $(RIOTBASE)/Makefile.include
