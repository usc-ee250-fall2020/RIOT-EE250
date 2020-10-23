APPLICATION = ee250_lab09
RIOTBASE ?= $(CURDIR)/../..
BOARD ?= native
USEMODULE += xtimer
USEMODULE += random
QUIET ?= 1

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

FEATURES_REQUIRED += periph_timer

include $(RIOTBASE)/Makefile.include
