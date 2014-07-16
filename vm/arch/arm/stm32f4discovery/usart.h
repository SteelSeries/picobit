#ifndef USART_H
#define USART_H

#define USART_RECV_BUF_LEN 256

#define SERIAL_PORT USART6

typedef struct __usart_buf
{
    volatile uint16_t head;
    volatile uint16_t tail;
    char buf[USART_RECV_BUF_LEN];
} usart_buf_t;

void usart_start(void);
void usart_putchar(char c);
void usart_write(const uint8_t *data, size_t len);
int usart_read(char *buf, size_t len);
int usart_getchar(char *ch);

#endif
