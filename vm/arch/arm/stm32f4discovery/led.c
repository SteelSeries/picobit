#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/stm32/gpio.h>

#include "led.h"

static uint16_t board_leds[BOARD_LEDS_NUM] = {
    BOARD_LED1_GPIO, BOARD_LED2_GPIO, BOARD_LED3_GPIO, BOARD_LED4_GPIO
};

void set_led(uint8_t led, uint8_t val)
{
    uint16_t gpio;

    if (!(gpio = (led < BOARD_LEDS_NUM) ? board_leds[led] : 0))
        return;

    if (val == 0)
        gpio_clear(BOARD_LED_PORT, gpio);
    else
        gpio_set(BOARD_LED_PORT, gpio);
}

void led_init(void)
{
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
        BOARD_LED1_GPIO|BOARD_LED2_GPIO| BOARD_LED3_GPIO|BOARD_LED4_GPIO);
}
