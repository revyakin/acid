#ifndef USART_H
#define USART_H

#include <stm32f10x.h>

#define USART_NO_DATA -1

void usart_init(void);
int usart_getc(void);
void usart_putc(char c);
void usart_puts(char * str);

#endif /* end of include guard: USART_H */
