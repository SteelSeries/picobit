#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/f4/pwr.h>
#include <libopencm3/stm32/rcc.h>

#include <printf.h>

#include "rtc.h"

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

#define RCC_BDCR_RTCSEL (8)

#define RCC_BDCR_RTCSEL_MASK (3 << RCC_BDCR_RTCSEL)
#define RCC_BDCR_RTCSEL_HSE  (3 << RCC_BDCR_RTCSEL)
#define RCC_BDCR_RTCSEL_LSI  (2 << RCC_BDCR_RTCSEL)

void board_rtc_init(void)
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

#if 0
void board_rtc_init(void)
{
    /* Enable write access to backup domain: RTC regs, RTC backup data regs,
     * backup SRAM
     */
    pwr_disable_backup_domain_write_protect();

    /* Checking magic */
//    printf("Magic: 0x%X\r\n", RTC_BKPXR(0));

    RCC_BDCR |= RCC_BDCR_BDRST;
    RCC_BDCR &= ~RCC_BDCR_BDRST;

    rtc_unlock();

    rcc_osc_on(HSE);
    rcc_wait_for_osc_ready(HSE);

    RCC_BDCR &= ~RCC_BDCR_RTCSEL_MASK;
    RCC_BDCR |= RCC_BDCR_RTCSEL_MASK;

//    printf("Unlocking.\r\n");
    /* set synch prescaler, using defaults for 1Hz out */
//    printf("Init mode.\r\n");

//    printf("=> %X\r\n", RTC_ISR);
    /* Enter init mode */
    RTC_ISR |= RTC_ISR_INIT;
//    printf("=> %X\r\n", RTC_ISR);
    while ((RTC_ISR & RTC_ISR_INITF) == 0);
//    printf("Init finished.\r\n");

    uint32_t sync = 255;
    uint32_t async = 127;
    rtc_set_prescaler(sync, async);

    /* exit init mode */
    RTC_ISR &= ~RTC_ISR_INIT;

    /* and finally enable the clock */
    RCC_BDCR |= RCC_BDCR_RTCEN;

    /* and write protect again */
    rtc_lock();

    printf("Wait for sync.\r\n");
    rtc_wait_for_synchro();
}
#endif

/* RTC init:
   1. Enable HSE for rcc
   2. Wait for RTC sync
   3. Unlock RTC registers
   4. Enter init mode
   5. Set misc options and pre-scaler
   6. Exit init mode
   7. Lock RTC registers
*/
