#include <stm32f10x.h>
#include "pwm.h"
#include "drive.h"
#include "usart.h"
#include "crc32.h"

#define STK_LOAD_VAL 300000 - 1

s16             freq        = 273;
s16             old_freq    = 0;
sine_param_t    sinep;
u32 counter;

uint32_t test_crc;
uint8_t data[] = "Our looooooong message";

uint8_t txBuffer[256];

#define START 'S'
#define END   'E'

void send_frame(uint8_t data[], uint8_t dataLength)
{
    uint8_t *frame = txBuffer;

    *(frame++) = START;
    *(frame++) = dataLength + 7;

    for (uint8_t i = 0; i < dataLength; i++) {
        *(frame++) = data[i];
    }

    *( (uint32_t *) frame) = crc32(txBuffer + 2, dataLength);
    frame += 4;

    *frame = END;

    tx_dma_start_transfer(txBuffer, dataLength + 7);
}

void init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;

    GPIOC->CRH &= ~GPIO_CRH_CNF9;
    GPIOC->CRH |= GPIO_CRH_MODE9;

    pwm_init();

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    SysTick->LOAD = STK_LOAD_VAL;
    SysTick->CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;

    usart_init();

    test_crc = crc32(data, sizeof(data));
}

uint8_t systick_flag = 0;

int main(void)
{
    init();
    __enable_irq();

    for(;;)
    {
        if (GPIOC->ODR & GPIO_ODR_ODR8)
            GPIOC->ODR &= ~GPIO_ODR_ODR8;

        asm("":::"memory");

        if (old_freq != freq)
        {
            old_freq = freq;

            sinep.amplitude_pwm = drive_vf_control(freq);
            sinep.freq_m        = freq;

            pwm_reconfigure(&sinep);
        }

//        if (usart_getch() != USART_NO_DATA) {
//            GPIOC->ODR ^= GPIO_ODR_ODR9;
//            tx_dma_start_transfer("Hello!\r\n", 8);
//        }

        if (systick_flag) {
            systick_flag = 0;

            send_frame(data, sizeof(data));
        }

    }

    return 0;
}
