include arch/defs.mk

ifdef CONFIG_ARCH_HOST
load-address := \#x8000
arch := arch/host
include arch/host/Makefile
endif

ifdef CONFIG_ARCH_ARM
load-address := \#x8008000
arch := arch/arm
include arch/arm/Makefile
endif
