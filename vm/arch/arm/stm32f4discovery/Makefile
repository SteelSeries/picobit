ldscript := stm32f4xx.ld

INCLUDE_DIRS = $(call unquote, $(CONFIG_LIBOPENCM3_PATH))/include/
LD_DIRS = $(call unquote, $(CONFIG_LIBOPENCM3_PATH))/lib/

INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

CFLAGS += -mcpu=cortex-m4 -mthumb -g -fno-hosted -nostdlib \
          -L$(arch)/lib -L$(LD_DIRS) -T$(ldscript) -Wl,-warn-common \
	  -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb-interwork -mfloat-abi=hard \
	  -DSTM32F4 -DPRINTF_LONG_SUPPORT

arch-$(CONFIG_ARM_BOARD_STM32_F4DISCOVERY)   += board.c time.c usart.c sdio.c \
						led.c primitives.c lcd.c \
						random.c error.c
stdlib-$(CONFIG_ARM_BOARD_STM32_F4DISCOVERY) += board.scm graphics.scm

arch-all: picobit.hex picobit.bin

picobit.hex: picobit.elf
	$(Q)$(ECHO) "Building VM hex..."
	$(OBJCOPY) -O ihex $^ $@

picobit.bin: picobit.elf
	$(Q)$(ECHO) "Building VM bin..."
	$(OBJCOPY) -O binary $^ $@

picobit.elf: $(obj-y) .config
	$(Q)$(ECHO) "Building VM elf..."
	$(CC) $(INCLUDE) $(CFLAGS) -o $@ $(obj-y) -lopencm3_stm32f4
