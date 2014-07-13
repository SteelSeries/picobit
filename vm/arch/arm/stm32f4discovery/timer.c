#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <printf.h>

#include "timer.h"

/* TODO:
 * Use RTC timer and look for overflow bit or use watchdog mechanism,
 * which is better?
 */

volatile uint32_t g_sys_timer = 0;

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

void sys_tick_handler(void)
{
    if (g_sys_timer > 0) g_sys_timer --;
}
/*
#define RTC_CNTH MMIO32(RTC_BASE + 0x18)
#define RTC_CNTL MMIO32(RTC_BASE + 0x1c)
#define RTC_CRL  MMIO32(RTC_BASE + 0x04)
#define RTC_CRL_SECF (1 << 0)

uint32_t rtc_get_val(void)
{
    uint16_t flags;
    uint16_t cnth, cntl, tmp;

    flags = irqsave();

    do {
        tmp  = RTC_CNTL;
        cnth = RTC_CNTH;
        cntl = RTC_CNTL;
    } while (cntl < tmp);

    irqrestore(flags);

    return (uint32_t)(cnth << 16) | (uint32_t)cntl;
}
*/
void sleep(unsigned msec)
{
    g_sys_timer = msec;
    while (g_sys_timer > 0);
}

void timer_init(void)
{
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    /* systick mhz */
    systick_set_reload(/*180 * 1000*/168000);
    systick_interrupt_enable();
    systick_counter_enable();
}
