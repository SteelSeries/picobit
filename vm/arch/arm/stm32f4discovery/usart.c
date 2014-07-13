#include <stdlib.h>
#include <stdint.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "usart.h"

void usart6_isr(void)
{
    static uint8_t data = 'A';

    /* Check if we were called because of RXNE. */
    if (((USART_CR1(USART6) & USART_CR1_RXNEIE) != 0) &&
        ((USART_SR(USART6) & USART_SR_RXNE) != 0)) {
        /* Retrieve the data from the peripheral. */
        data = usart_recv(USART6);
        /* Enable transmit interrupt so it sends back the data. */
        usart_enable_tx_interrupt(USART6);
    }

    /* Check if we were called because of TXE. */
    if (((USART_CR1(USART6) & USART_CR1_TXEIE) != 0) &&
        ((USART_SR(USART6) & USART_SR_TXE) != 0)) {
        /* Put data into the transmit register. */
        usart_send(USART6, data);
        /* Disable the TXE interrupt as we don't need it anymore. */
        usart_disable_tx_interrupt(USART6);
    }
}

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

    /* Enable RX interrupts */
    usart_enable_rx_interrupt(USART6);

    usart_enable(USART6);
}
