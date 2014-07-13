#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <picobit.h>
#include <dispatch.h>
#include <primitives.h>
#include <bignum.h>
#include <printf.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f4/flash.h>
#include <libopencm3/stm32/f4/pwr.h>

#include "board.h"
#include "rcc.h"
#include "usart.h"
#include "timer.h"
#include "led.h"

static inline uint16_t irqsave(void)
{
    uint16_t primask;

    __asm__ __volatile__
        (
            "\tmrs    %0, primask\n"
            "\tcpsid  i\n"
            : "=r" (primask)
            :
            : "memory");

    return primask;
}

static inline void irqrestore(uint16_t flags)
{
    __asm__ __volatile__
        (
            "\ttst    %0, #1\n"
            "\tbne    1f\n"
            "\tcpsie  i\n"
            "1:\n"
            :
            : "r" (flags)
            : "memory");
}

void halt_with_error (void)
{
    set_led(BOARD_LED_RED, 1);
    printf("\n\n\n\nError, halt!\n\n\n\n");
    while(1);
}

void __nvic_enable(void)
{
    /* USB CDC/ACM */
    nvic_enable_irq(NVIC_OTG_FS_IRQ);
    /* SPI RX */
    nvic_enable_irq(NVIC_DMA1_STREAM3_IRQ);
    /* SPI TX */
    nvic_enable_irq(NVIC_DMA1_STREAM4_IRQ);
}

void put_char(void *p, char c)
{
    usart_putchar(c);
}

void main ()
{
    /* Interrupts */
    __nvic_enable();

    rcc_enable();

    led_init();
    timer_init();
    usart_start();

    init_printf(NULL, put_char);

    printf("Starting interpreter.\r\n");

    interpreter();
}

/* GPIOD - AHB2 */
/* base: 0x40000000 + 0x08000000 + 0x0C00 */
