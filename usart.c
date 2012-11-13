#include "usart.h"
#include <stm32f10x.h>

#define USART_RX_BUF_SIZE 64

u8 rx_ring_buffer[USART_RX_BUF_SIZE];
u16 p_next;

static void usart_gpio_init(void)
{
    /* Enable PORTC and AFIO module clocking */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* remap USART3 Rx to PC11, Tx to PC10 */
    AFIO->MAPR &= ~AFIO_MAPR_USART3_REMAP;
    AFIO->MAPR |=  AFIO_MAPR_USART3_REMAP_PARTIALREMAP;

    /* Push-pull output, alternative function */
    GPIOC->CRH |=  GPIO_CRH_MODE10;
    GPIOC->CRH &= ~GPIO_CRH_CNF10;
    GPIOC->CRH |=  GPIO_CRH_CNF10_1;

    /* Pulled-up input */
    GPIOC->CRH &= ~GPIO_CRH_MODE11;
    GPIOC->CRH &= ~GPIO_CRH_CNF11;
    GPIOC->CRH |=  GPIO_CRH_CNF11_1;
    GPIOC->ODR |=  GPIO_ODR_ODR11;
}

static void usart_dma_init(void)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    DMA1_Channel3->CPAR = &(USART3->DR);
    DMA1_Channel3->CMAR = rx_ring_buffer;
    DMA1_Channel3->CNDTR = USART_RX_BUF_SIZE;

    p_next = 0;

    DMA1_Channel3->CCR |= DMA_CCR3_MINC | DMA_CCR3_CIRC | DMA_CCR3_EN;
}

void usart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    USART3->CR1 |= USART_CR1_UE;

    /* 460.8 kbps */
    USART3->BRR = (3 << 4) + 4;

    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE;
    USART3->CR3 |= USART_CR3_DMAR;

    usart_dma_init();
    usart_gpio_init();
}

u16 usart_recv_count(void)
{
    u16 p_curr = USART_RX_BUF_SIZE - DMA1_Channel3->CNDTR;

    return (p_curr >= p_next) ? p_curr - p_next :
        USART_RX_BUF_SIZE - (p_next - p_curr);
}

void usart_recv_buf(u8 *buf, u16 bufsize, u16 *readed)
{
    u16 recv_count = usart_recv_count();
    recv_count = (bufsize > recv_count) ? recv_count : bufsize;

    *readed = recv_count;

    for (;recv_count > 0; recv_count --) {
        *buf = rx_ring_buffer[p_next];
        buf++;

        if (++p_next >= USART_RX_BUF_SIZE)
            p_next = 0;
    }
}

