#include "usart.h"
#include <stm32f10x.h>

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
}

void usart_rx_dma_start(uint8_t pBuffer[], uint16_t bufferLength)
{
    DMA1_Channel3->CCR &= ~DMA_CCR3_EN;         /* Disable DMA */

    DMA1_Channel3->CPAR  = (int) &(USART3->DR);       /* From register */
    DMA1_Channel3->CMAR  = (int) pBuffer;             /* To memory address */
    DMA1_Channel3->CNDTR = bufferLength;        /* Number of bytes */

    /* DMA start */
    DMA1_Channel3->CCR |= DMA_CCR3_MINC | DMA_CCR3_EN;
}

void usart_rx_dma_stop(void)
{
    DMA1_Channel3->CCR &= ~DMA_CCR3_EN;         /* Disable DMA */
}

uint16_t usart_rx_cndtr(void)
{
    return DMA1_Channel3->CNDTR;
}

void usart_tx_dma_start(uint8_t pBuffer[], uint16_t bufferLength)
{
    DMA1_Channel2->CCR &= ~DMA_CCR2_EN;     /* Disable DMA */

    DMA1_Channel2->CPAR  = (int) &(USART3->DR);
    DMA1_Channel2->CMAR  = (int) pBuffer;
    DMA1_Channel2->CNDTR = bufferLength;

    USART3->SR &= ~USART_SR_TC;

    DMA1_Channel2->CCR |= DMA_CCR2_MINC | DMA_CCR2_DIR | DMA_CCR2_EN;
}

void usart_tx_dma_stop(void)
{
    DMA1_Channel2->CCR &= ~DMA_CCR2_EN;     /* Disable DMA */
}

void usart_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    USART3->CR1 |= USART_CR1_UE;

    usart_dma_init();
    usart_gpio_init();

    //USART3->BRR = (3 << 4) + 4;                     /* 460.8 Kbps */
    USART3->BRR = (156 << 4) + 4;                     /* 9600 bps */
    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE; // | USART_CR1_IDLEIE | USART_CR1_TCIE;
    USART3->CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;
}

