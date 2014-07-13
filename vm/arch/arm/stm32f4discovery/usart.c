#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

void usart_print(const uint8_t *data, size_t len)
{
    while (len--)
        usart_send_blocking(USART6, *data++);
}

void usart_putchar(char c)
{
    usart_send_blocking(USART6, c);
}

void usart_start(void)
{
    /* GPIO init */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6); /* Tx */
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO7); /* Rx */
    gpio_set_output_options(GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO7);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO6);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO7);

    /* USART configuration */
    usart_set_baudrate(USART6, 115200);
    usart_set_databits(USART6, 8);
    usart_set_stopbits(USART6, USART_STOPBITS_1);
    usart_set_mode(USART6, USART_MODE_TX_RX);
    usart_set_parity(USART6, USART_PARITY_NONE);
    usart_set_flow_control(USART6, USART_FLOWCONTROL_NONE);

    usart_enable(USART6);
}
