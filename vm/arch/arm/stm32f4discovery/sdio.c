#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/sdio.h>

#include "sdio.h"

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
}
