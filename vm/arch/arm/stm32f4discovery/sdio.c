#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/sdio.h>

#include "sdio.h"

#define SDIO_INIT_CLKDIV (118 << SDIO_CLKCR_CLKDIV_SHIFT)
#define SDIO_CLKCR_RISINGEDGE 0

void sdio_init(void)
{
    /* CLK */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO12);
    /* D0 */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO8);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO8);
    /* D1 */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO9);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO9);
    /* D2 */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO10);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO10);
    /* D3 */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO11);
    gpio_set_af(GPIOC, GPIO_AF12, GPIO11);

    /* CMD */
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO2);
    gpio_set_af(GPIOD, GPIO_AF12, GPIO2);
    gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO2);

    /* CD */
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
    gpio_set_af(GPIOD, GPIO_AF5, GPIO3);

    sdio_reset();
}
void sdio_reset(void)
{
    SDIO_POWER  = 0;
    SDIO_CLKCR  = 0;
    SDIO_ARG    = 0;
    SDIO_CMD    = 0;
    SDIO_DTIMER = 0;
    SDIO_DLEN   = 0;
    SDIO_DCTRL  = 0;
    SDIO_ICR    = 0x00c007ff;
    SDIO_MASK   = 0;

    SDIO_CLKCR = (SDIO_CLKCR_CLKEN | SDIO_INIT_CLKDIV |
                  SDIO_CLKCR_RISINGEDGE | SDIO_CLKCR_WIDBUS_1);
    SDIO_POWER = SDIO_POWER_PWRCTRL_PWRON;
}

int stm32_sendcmd(uint32_t sdio_cmd, uint32_t cmd, uint32_t resp, uint32_t arg)
{
    uint32_t regval;

    SDIO_ARG = arg;

    regval = SDIO_CMD;
    regval &= ~(SDIO_CMD_CMDINDEX_MSK|SDIO_CMD_WAITRESP_LONG|
                SDIO_CMD_WAITPEND|SDIO_CMD_CPSMEN|SDIO_CMD_WAITINT);
    regval |= resp; /* SHORT/LONG/NONE */

    regval |= (cmd|SDIO_CMD_CPSMEN);

    SDIO_ICR = (SDIO_ICR_CMDSENTC|SDIO_ICR_DBCKENDC|SDIO_ICR_CTIMEOUTC|
                SDIO_ICR_CCRCFAILC|SDIO_ICR_CMDRENDC);
    SDIO_CMD = regval;

    return 0;
}
