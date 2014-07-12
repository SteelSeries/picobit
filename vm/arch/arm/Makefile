ifdef CONFIG_ARM_STM32VLDISCOVERY
arch := $(arch)/stm32vldiscovery
endif

ifdef CONFIG_ARM_STM32F4DISCOVERY
arch := $(arch)/stm32f4discovery
endif

CC      = $(CROSS)gcc
OBJCOPY = $(CROSS)objcopy
LD      = $(CROSS)ld
ECHO    = echo
Q      ?= @

include $(arch)/Makefile
