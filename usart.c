#include "usart.h"
#include <stm32f10x.h>

static char rx_ringbuffer[USART_RX_BUF_SIZE];
static int  rx_read_ptr;

static char tx_buffer[USART_TX_BUF_SIZE];
static int  tx_write_ptr;

static void gpio_init(void)
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

static void dma_init(void)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

static void rx_dma_setup(void)
{
    DMA1_Channel3->CPAR  = (int) &(USART3->DR);
    DMA1_Channel3->CMAR  = (int) rx_ringbuffer;
    DMA1_Channel3->CNDTR = USART_RX_BUF_SIZE;

    DMA1_Channel3->CCR |= DMA_CCR3_MINC | DMA_CCR3_CIRC;

    rx_read_ptr = 0;
}

static void rx_dma_start_transfer(void)
{
    DMA1_Channel3->CCR |= DMA_CCR3_EN;
}

static void rx_dma_stop_transfer(void)
{
    DMA1_Channel3->CCR &= ~DMA_CCR3_EN;
}

static int rx_dma_complete_flag(void)
{
    return DMA1->ISR & DMA_ISR_TCIF3;
}

static void rx_dma_clear_complete_flag(void)
{
    DMA1->IFCR |= DMA_IFCR_CTCIF3;
}

static int rx_dma_current_address(void)
{
    return USART_RX_BUF_SIZE - DMA1_Channel3->CNDTR;
}

static int rx_get_received_count(void)
{
    int dma_curr_address = rx_dma_current_address();

    if (rx_dma_complete_flag()) {
        rx_dma_clear_complete_flag();

        if (rx_read_ptr < dma_curr_address) {

            rx_dma_stop_transfer();
            rx_dma_setup();
            rx_dma_start_transfer();

            return USART_RX_FIFO_OVERLOAD;
        }
    }

    return (rx_read_ptr <= dma_curr_address) ? dma_curr_address - rx_read_ptr :
        USART_RX_BUF_SIZE - (rx_read_ptr - dma_curr_address);
}

int tx_dma_start_transfer(char *buf, int size)
{
    if (DMA1_Channel2->CNDTR != 0)
        return USART_TX_BUSY;

    DMA1_Channel2->CCR &= ~DMA_CCR2_EN;

    DMA1_Channel2->CPAR  = (int) &(USART3->DR);
    DMA1_Channel2->CMAR  = (int) buf;
    DMA1_Channel2->CNDTR = size;

    DMA1_Channel2->CCR |= DMA_CCR2_MINC | DMA_CCR2_DIR;

    USART3->SR &= ~USART_SR_TC;

    DMA1_Channel2->CCR |= DMA_CCR2_EN;

    return 0;
}

void usart_init(void)
{
    gpio_init();

    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    USART3->CR1 |= USART_CR1_UE;

    USART3->CR3 |= USART_CR3_DMAR | USART_CR3_DMAT;

    dma_init();
    rx_dma_setup();
    rx_dma_start_transfer();

    /* 460.8 kbps */
    USART3->BRR = (3 << 4) + 4;

    USART3->CR1 |= USART_CR1_RE | USART_CR1_TE;
}

int usart_getch(void)
{
    switch (rx_get_received_count()) {
        case 0:
            return USART_NO_DATA;

        case USART_RX_FIFO_OVERLOAD:
            return USART_RX_FIFO_OVERLOAD;
    }

    char ch = rx_ringbuffer[rx_read_ptr];

    if (++rx_read_ptr >= USART_RX_BUF_SIZE)
        rx_read_ptr = 0;

    return (int) ch;
}

static int usart_flush(void)
{
    int status = tx_dma_start_transfer(tx_buffer, tx_write_ptr);
    tx_write_ptr = 0;
    return status;
}

int usart_putch(char c)
{
    tx_buffer[tx_write_ptr] = c;    

    if (++tx_write_ptr >= USART_TX_BUF_SIZE) {
        return usart_flush();
    }

    return 0;
}

