#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/cm3/systick.h>

#include "timer.h"

/* TODO:
 * Use RTC timer and look for overflow bit or use watchdog mechanism,
 * which is better?
 */

volatile uint32_t g_sys_timer = 0;

void sys_tick_handler(void)
{
    if (g_sys_timer > 0) g_sys_timer --;
}

void sleep(unsigned msec)
{
    g_sys_timer = msec;
    while (g_sys_timer > 0);
}

void timer_init(void)
{
    systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);
    /* systick mhz */
    systick_set_reload(/*180 * 1000*/168000);
    systick_interrupt_enable();
    systick_counter_enable();
}
