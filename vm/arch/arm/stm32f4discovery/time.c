#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <printf.h>

#include "time.h"

#define RCC_BDCR_RTCSEL (8)

#define RCC_BDCR_RTCSEL_MASK (3 << RCC_BDCR_RTCSEL)
#define RCC_BDCR_RTCSEL_HSE  (3 << RCC_BDCR_RTCSEL)
#define RCC_BDCR_RTCSEL_LSI  (2 << RCC_BDCR_RTCSEL)

#define BCD2BIN(val) (int)((((val) >> 4) * 10) + ((val) & 0x0F))

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
    g_sys_timer++;
}

/* RTC configuration flow:
 * - After sys reset RTC regs are wr protected with the DBP bit of the PWR_CR
 *   register. DBP must be set to enable RTC wr access.
 * - After domain reset all RTC regs are wr protected, enable wr by writing
 *   key values to RTC_WPR:
 *   0xCA to RTC_WPR
 *   0x53 to RTC_WPR
 *
 * Calendar init and conf
 * - Set INIT bit to 1  in RTC_ISR reg to enter init mode.
 * - Poll INITF bit in RTC_ISR reg. When INITF bit is set init mode
 *   entered(takes ~2 RTCCLK clock cycles)
 * - To generate 1Hz clock for the cal counter program sync prescaler factor
 *   in RTC_PREP and then async prescaler factor. Even if one value must be
 *   changed 2 writes must be done anyway.
 * - Load initial time values in shadow regs(RTC_TR, RTC_DR), config time
 *   format 12/24h through the FMT bit in RTC_CR reg
 * - Exit init mode by clearing INIT bit. Counting restarts in 4 RTCCLK cycles
 *
 * To read calendar after init soft must first check that RSF flag is set in
 * RTC_ISR reg
 */

void rtc_init(void)
{
    pwr_disable_backup_domain_write_protect();

    /* Clear reset flags */
    RCC_CSR |= RCC_CSR_RMVF;
    RCC_CSR &= ~RCC_CSR_RMVF;
    /* LSI: LSION */
    RCC_CSR |= RCC_CSR_LSION;
    while ((RCC_CSR & RCC_CSR_LSIRDY) == 0);

    /* reset RTC */
    RCC_BDCR |= RCC_BDCR_BDRST;
    RCC_BDCR &= ~RCC_BDCR_BDRST;
    /* Enable HSE for RTC clock */
    RCC_BDCR &= ~RCC_BDCR_RTCSEL_MASK;
    RCC_BDCR |= RCC_BDCR_RTCSEL_LSI;
    /* Actually enable */
    RCC_BDCR |= RCC_BDCR_RTCEN;

    rtc_unlock();

    /* Command init mode and wait while we are at it */
    RTC_ISR |= RTC_ISR_INIT;
    while ((RTC_ISR & RTC_ISR_INITF) == 0);

    rtc_set_prescaler(255, 127);

    /* Leave init mode */
    RTC_ISR &= ~RTC_ISR_INIT;

    rtc_lock();

    rtc_wait_for_synchro();
}

void sleep(unsigned msec)
{
    uint32_t start_tick;

    start_tick = g_sys_timer + msec;
    while (start_tick > g_sys_timer);
}

uint32_t get_systick(void)
{
    return g_sys_timer;
}

void timer_init(void)
{
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    /* systick mhz */
    systick_set_reload(168000);
    systick_interrupt_enable();
    systick_counter_enable();
}

void gettime(struct tm *tp)
{
    uint32_t tr, dr;

    /* XXX: day may change while time will be for the previous day */
    tr = (uint32_t)RTC_TR;
    dr = (uint32_t)RTC_DR;

    tp->tm_sec  = BCD2BIN((tr & ((RTC_TR_SU_MASK  << RTC_TR_SU_SHIFT)   |
                                 (RTC_TR_ST_MASK  << RTC_TR_ST_SHIFT)))
                          >> RTC_TR_SU_SHIFT);
    tp->tm_min  = BCD2BIN((tr & ((RTC_TR_MNU_MASK << RTC_TR_MNU_SHIFT)  |
                                 (RTC_TR_MNT_MASK << RTC_TR_MNT_SHIFT)))
                          >> RTC_TR_MNU_SHIFT);
    tp->tm_hour = BCD2BIN((tr & ((RTC_TR_HU_MASK  << RTC_TR_HU_SHIFT)   |
                                 (RTC_TR_HT_MASK  << RTC_TR_HT_SHIFT)))
                          >> RTC_TR_HU_SHIFT);

    tp->tm_mday = BCD2BIN((dr & ((RTC_DR_DU_MASK << RTC_DR_DU_SHIFT) |
                                 (RTC_DR_DT_MASK << RTC_DR_DT_SHIFT)))
                          >> RTC_DR_DU_SHIFT);
    tp->tm_mon  = BCD2BIN((dr & ((RTC_DR_MU_MASK << RTC_DR_DU_SHIFT) |
                                 (RTC_DR_MT << RTC_DR_MT_SHIFT)))
                          >> RTC_DR_MU_SHIFT) - 1;
    tp->tm_year = BCD2BIN((dr & ((RTC_DR_YU_MASK << RTC_DR_YU_SHIFT) |
                                 (RTC_DR_YT_MASK << RTC_DR_YT_SHIFT)))
                          >> RTC_DR_YU_SHIFT) + 100;
}

/* TODO: Needs to be completly rewritten */
uint32_t mktime(const struct tm *tp)
{
    int dy, m, y, tmp;

    uint32_t jd;

    dy = tp->tm_mon / 12;
    m = 1 + tp->tm_mon - 12 * dy;
    y = tp->tm_year + 1900 + dy;

    if (m < 1) {
        m += 12;
        y -= 1;
    }

    tmp = (m <= 2) ? -1 : 0;

    jd =  ((1461 * (y + 4800 + tmp)) / 4 + (367 * (m - 2 - 12 * tmp)) / 12 -
           (3 * ((y + 4900 + tmp) / 100)) / 4 + tp->tm_mday - 32075) - 2440588;

    return ((jd * 24 + tp->tm_hour) * 60 + tp->tm_min) * 60 + tp->tm_sec;
}
