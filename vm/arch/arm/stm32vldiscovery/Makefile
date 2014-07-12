ifdef CONFIG_ARM_STM32VLDISCOVERY
ldscript := stm32f100rb.ld
endif

CFLAGS += -mcpu=cortex-m3 -mthumb -g -fno-hosted -nostdlib \
          -L$(arch)/lib -T$(ldscript) -Wl,-warn-common

arch-y += init.s

arch-$(CONFIG_ARM_BOARD_STM32_VLDISCOVERY)   += board-vldiscovery.c
stdlib-$(CONFIG_ARM_BOARD_STM32_VLDISCOVERY) += stdlib-vldiscovery.scm

load-address := \#x8008000

arch-all: picobit.hex

picobit.hex: picobit.elf
	$(OBJCOPY) -O ihex $^ $@

picobit.elf: $(obj-y) .config
	$(CC) $(CFLAGS) -o $@ $(obj-y)
