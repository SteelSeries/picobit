#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/cm3/systick.h>

#include "timer.h"

static volatile uint32_t g_system_timer = 0;

void sys_tick_handler(void)
{
    g_system_timer++;
}

uint32_t get_systick(void)
{
    return g_system_timer;
}

void timer_init(void)
{
    systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);
    /* systick mhz */
    systick_set_reload(/*180 * 1000*/168000);
    systick_interrupt_enable();
    systick_counter_enable();
}
