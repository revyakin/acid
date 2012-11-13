#ifndef USART_H
#define USART_H

#include <stm32f10x.h>

void usart_init(void);
void usart_recv_buf(u8 *buf, u16 bufsize, u16 *readed);

#endif /* end of include guard: USART_H */
