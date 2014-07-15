#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <picobit.h>
#include <dispatch.h>
#include <primitives.h>
#include <bignum.h>
#include <printf.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include "board.h"
#include "usart.h"
#include "time.h"
#include "led.h"

void halt_with_error (void)
{
    set_led(BOARD_LED_BLUE, 1);
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
    /* USART6 IRQ. */
    nvic_enable_irq(NVIC_USART6_IRQ);
    /* RTC Wakeup and alarm IRQ's. */
    nvic_enable_irq(NVIC_RTC_WKUP_IRQ);
    nvic_enable_irq(NVIC_RTC_ALARM_IRQ);
}

void put_char(void *p, char c)
{
    usart_putchar(c);
}

#ifdef CLOCK_HSI
static void __clock_setup_hsi(void)
{
    clock_scale_t scale = {
        .pllm = 4,
        .plln = 84,
        .pllp = 2,
        .pllq = 7,
        .hpre = RCC_CFGR_HPRE_DIV_NONE,
        .ppre1 = RCC_CFGR_PPRE_DIV_4,
        .ppre2 = RCC_CFGR_PPRE_DIV_2,
        .power_save = 1,
        .flash_config = FLASH_ACR_ICE | FLASH_ACR_DCE | FLASH_ACR_LATENCY_5WS,
        .apb1_frequency = 168000000ul/4,
        .apb2_frequency = 168000000ul/2,
    };

    /* Enable internal high-speed oscillator. */
    rcc_osc_on(HSI);
    rcc_wait_for_osc_ready(HSI);
    /* Select HSI as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_HSI);
    rcc_wait_for_sysclk_status(HSI);

    /* Enable/disable high performance mode */
    if (!scale.power_save) {
        pwr_set_vos_scale(0);
    } else {
        pwr_set_vos_scale(1);
    }

    rcc_osc_off(PLL);
    while ((RCC_CR & RCC_CR_PLLRDY) != 0);

    rcc_set_main_pll_hsi(scale.pllm, scale.plln, scale.pllp, scale.pllq);

    rcc_osc_on(PLL);
    /* Enable PLL oscillator and wait for it to stabilize. */
    rcc_wait_for_osc_ready(PLL);

    rcc_set_hpre(scale.hpre);
    rcc_set_ppre1(scale.ppre1);
    rcc_set_ppre2(scale.ppre2);

    /* Configure flash settings. */
    flash_set_ws(scale.flash_config);
    /* Select PLL as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
    /* Wait for PLL clock to be selected. */
    rcc_wait_for_sysclk_status(PLL);

    /* Set the peripheral clock frequencies used. */
    rcc_ppre1_frequency = scale.apb1_frequency;
    rcc_ppre2_frequency = scale.apb2_frequency;
}
#endif

void __rcc_enable(void)
{
#ifdef CLOCK_HSI
    __clock_setup_hsi(void);
#else
    /* Enable LSI for RTC */
    rcc_osc_on(LSI);
    rcc_wait_for_osc_ready(LSI);

    rcc_osc_bypass_enable(HSE);
    rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_168MHZ]);
#endif
    rcc_peripheral_enable_clock(&RCC_APB1ENR,
                                RCC_APB1ENR_SPI2EN|RCC_APB1ENR_PWREN);
    rcc_peripheral_enable_clock(&RCC_APB2ENR,
                                RCC_APB2ENR_SYSCFGEN|RCC_APB2ENR_USART6EN);
    rcc_peripheral_enable_clock(&RCC_AHB1ENR,
                                RCC_AHB1ENR_IOPAEN|RCC_AHB1ENR_IOPBEN|
                                RCC_AHB1ENR_IOPCEN|RCC_AHB1ENR_IOPDEN|
                                RCC_AHB1ENR_DMA1EN);
    rcc_peripheral_enable_clock(&RCC_AHB2ENR, RCC_AHB2ENR_OTGFSEN);
//    rcc_periph_clock_enable(RCC_PWR);
//    rcc_periph_clock_enable(RCC_SPI2);
//    rcc_periph_clock_enable(RCC_GPIOA);
//    rcc_periph_clock_enable(RCC_GPIOB);
//    rcc_periph_clock_enable(RCC_GPIOC);
//    rcc_periph_clock_enable(RCC_GPIOD);
    rcc_periph_clock_enable(RCC_RTC);
    rcc_periph_clock_enable(RCC_TIM6);
    rcc_periph_clock_enable(RCC_BKPSRAM);
}

void main ()
{
    /* Interrupts */
    __nvic_enable();
    __rcc_enable();

    rtc_init();
    timer_init();

    led_init();
    usart_start();

    init_printf(NULL, put_char);

    printf("  RCC_CR: 0x%X\r\n", RCC_CR);
    printf(" RCC_CSR: 0x%X\r\n", RCC_CSR);
    printf("RCC_BDCR: 0x%x\r\n", RCC_BDCR);
    printf("RCC_CFGR: 0x%x\r\n", RCC_CFGR);
    printf(" RCC_CIR: 0x%x\r\n", RCC_CIR);
    printf("Waiting for INITF.\r\n");

    printf("Starting interpreter.\r\n");

    interpreter();
}

/* GPIOD - AHB2 */
/* base: 0x40000000 + 0x08000000 + 0x0C00 */
