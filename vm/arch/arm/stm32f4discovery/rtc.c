#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/f4/pwr.h>
#include <libopencm3/stm32/rcc.h>

#include "rtc.h"

/* XXX: STM32F4Discovery doesnt have functional LSE, so skip RTC for now :( */

#define RCC_BDCR_RTCSEL (1 << 3)
#define RCC_BDCR_RTCSEL_MASK (3 << RCC_BDCR_RTCSEL)

void rtc_init(void)
{
    pwr_disable_backup_domain_write_protect();
    rcc_osc_on(LSE);

    rcc_wait_for_osc_ready(LSE);

    RCC_BDCR &= ~RCC_BDCR_RTCSEL_MASK;
    RCC_BDCR |= RCC_BDCR_RTCSEL;

    RCC_BDCR |= RCC_BDCR_RTCEN;
}
