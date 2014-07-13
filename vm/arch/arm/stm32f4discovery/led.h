#ifndef LED_H
#define LED_H

#define BOARD_LEDS_NUM 4

#define BOARD_LED_PORT GPIOD

#define BOARD_LED1_GPIO GPIO12
#define BOARD_LED2_GPIO GPIO13
#define BOARD_LED3_GPIO GPIO14
#define BOARD_LED4_GPIO GPIO15

enum {
    BOARD_LED_GREEN = 0,
    BOARD_LED_ORANGE,
    BOARD_LED_RED,
    BOARD_LED_BLUE
};

void set_led(uint8_t led, uint8_t val);
void led_init(void);

#endif
