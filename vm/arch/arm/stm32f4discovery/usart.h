#ifndef USART_H
#define USART_H

void usart_start(void);
void usart_putchar(char c);
void usart_print(const uint8_t *data, size_t len);

#endif
