#ifndef USART_H
#define USART_H

#include <stm32f10x.h>

#define USART_RX_BUF_SIZE      32
#define USART_NO_DATA          -1
#define USART_RX_FIFO_OVERLOAD -2

void usart_init(void);
int  usart_getch(void);

#endif /* end of include guard: USART_H */
