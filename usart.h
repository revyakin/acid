#ifndef USART_H
#define USART_H

#include <stm32f10x.h>

#define USART_NO_DATA -1

void usart_init(void);
void usart_rx_dma_start(uint8_t pBuffer[], uint16_t bufferLength);
void usart_rx_dma_stop(void);
uint16_t usart_rx_cndtr(void);
void usart_tx_dma_start(uint8_t pBuffer[], uint16_t bufferLength);
void usart_tx_dma_stop(void);

#endif /* end of include guard: USART_H */
