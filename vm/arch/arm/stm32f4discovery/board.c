#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <picobit.h>
#include <dispatch.h>
#include <primitives.h>
#include <bignum.h>
#include <printf.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/f4/flash.h>
#include <libopencm3/stm32/f4/pwr.h>

void usart_print(const uint8_t *data, size_t len)
{
    while (len--)
        usart_send_blocking(USART6, *data++);
}

void print(const char *msg, size_t len)
{
    usart_print((const uint8_t *)msg, len);
}

void put_char(void *p, char c)
{
    usart_send_blocking(USART6, c);
}

void switch_led(int led)
{
    uint16_t gpio;
    volatile uint32_t val;

    switch (led) {
    case 0:
        gpio = GPIO12;
        break;
    case 1:
        gpio = GPIO13;
        break;
    case 2:
        gpio = GPIO14;
        break;
    case 3:
    default:
        gpio = GPIO15;
        break;
    }

    val = gpio_get(GPIOD, gpio);
    if (val == 0)
        gpio_set(GPIOD, gpio);
    else
        gpio_clear(GPIOD, gpio);
}


void halt_with_error ()
{
    gpio_set(GPIOD, GPIO14);
    printf("\n\n\n\nError, halt!\n\n\n\n");
    while(1);
}

PRIMITIVE_UNSPEC(#%sleep, arch_sleep, 1)
{
	volatile static int a, b;

	a1 = decode_int (arg1);

	for(a = 0; a < a1; a++) {
		for(b = 0; b < 1000; b++) {
			__asm__ __volatile__("nop");
		}
	}

	arg1 = OBJ_FALSE;
}

PRIMITIVE_UNSPEC(#%set-led!, arch_set_led, 1)
{
    a1 = decode_int(arg1);

    if (a1 == 0)
        gpio_set(GPIOD, GPIO12);
    else
        gpio_clear(GPIOD, GPIO12);

    arg1 = OBJ_FALSE;
}

//#if 0
static void __clock_setup_hsi(const clock_scale_t *clock)
{
	/* Enable internal high-speed oscillator. */
	rcc_osc_on(HSI);
	rcc_wait_for_osc_ready(HSI);
	/* Select HSI as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_HSI);
        rcc_wait_for_sysclk_status(HSI);

	/* Enable/disable high performance mode */
	if (!clock->power_save) {
		pwr_set_vos_scale(0);
	} else {
		pwr_set_vos_scale(1);
	}

        rcc_osc_off(PLL);
        while ((RCC_CR & RCC_CR_PLLRDY) != 0);

	rcc_set_main_pll_hsi(clock->pllm, clock->plln,
                             clock->pllp, clock->pllq);

        rcc_osc_on(PLL);
	/* Enable PLL oscillator and wait for it to stabilize. */
	rcc_wait_for_osc_ready(PLL);

	rcc_set_hpre(clock->hpre);
	rcc_set_ppre1(clock->ppre1);
	rcc_set_ppre2(clock->ppre2);

	/* Configure flash settings. */
	flash_set_ws(clock->flash_config);
	/* Select PLL as SYSCLK source. */
	rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	/* Wait for PLL clock to be selected. */
	rcc_wait_for_sysclk_status(PLL);

	/* Set the peripheral clock frequencies used. */
	rcc_ppre1_frequency = clock->apb1_frequency;
	rcc_ppre2_frequency = clock->apb2_frequency;
}
//#endif
void rcc_enable(void)
{
    rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_SPI2EN);
    rcc_peripheral_enable_clock(&RCC_APB2ENR,
                                RCC_APB2ENR_SYSCFGEN|RCC_APB2ENR_USART6EN);
    rcc_peripheral_enable_clock(&RCC_AHB1ENR,
                                RCC_AHB1ENR_IOPAEN|RCC_AHB1ENR_IOPBEN|
                                RCC_AHB1ENR_IOPCEN|RCC_AHB1ENR_IOPDEN);
    rcc_peripheral_enable_clock(&RCC_AHB2ENR, RCC_AHB2ENR_OTGFSEN);
    rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_DMA1EN);
}

void __gpio_init(void)
{
    /* LED's */
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                    GPIO12 | GPIO13 | GPIO14 | GPIO15);
    /* USART */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6); /* Tx */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7); /* Rx */
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO7);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO6);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO7);
}

void init(void)
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

    __clock_setup_hsi(&scale);
    rcc_enable();
    __gpio_init();
}

void usart_start(void)
{
    /* USART configuration */
    usart_set_baudrate(USART6, 115200);
    usart_set_databits(USART6, 8);
    usart_set_stopbits(USART6, USART_STOPBITS_1);
    usart_set_mode(USART6, USART_MODE_TX_RX);
    usart_set_parity(USART6, USART_PARITY_NONE);
    usart_set_flow_control(USART6, USART_FLOWCONTROL_NONE);

    usart_enable(USART6);
}


void main ()
{
    init();
    usart_start();

    init_printf(NULL,put_char);

#if 0
    volatile int i;
    while (1) {
        for (i = 0; i < 1000000; i++)
            __asm__ __volatile__("nop");
        switch_led(0);
        switch_led(1);
        for (i = 0; i < 1000000; i++)
            __asm__ __volatile__("nop");
        printf("Test: %X\r\n", 6);
    }
#endif
    printf("Starting interpreter.\r\n");
    interpreter();
}

/* GPIOD - AHB2 */
/* base: 0x40000000 + 0x08000000 + 0x0C00 */
