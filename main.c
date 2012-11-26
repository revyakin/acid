#include <stm32f10x.h>

#include "pwm.h"
#include "sine.h"
#include "drive.h"
#include "usart.h"
#include "modbus.h"

#define STK_LOAD_VAL 30000 - 1

sine_param_t sinep;

extern uint16_t mbRegs[16];

void init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;
    GPIOC->CRH &= ~GPIO_CRH_CNF9;
    GPIOC->CRH |= GPIO_CRH_MODE9;

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

//    NVIC_SetPriority(USART3_IRQn, 15);
 //   NVIC_EnableIRQ(USART3_IRQn);

    SysTick->LOAD = STK_LOAD_VAL;
    SysTick->CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;

    pwm_init();
    sine_reset();

    pwm_output_enable();
    usart_init();
    modbus_init();
}

u8  sampling_time_flag = 0;

static unsigned int abs(int x)
{
    return (x < 0) ? (unsigned int) (-x) : (unsigned int) x;
}

static unsigned int is_negative(int x)
{
    return (x < 0) ? 1 : 0;
}

int open_loop_acceleration(int reference, int acceleration)
{
    static int current;

    if (current == reference)
        return current;
    
    if (abs(current - reference) < acceleration) {
        current = reference;
        return current;
    }

    if (current > reference)
    {
        current -= acceleration;
    } else {
        current += acceleration;
    }

    return current;
}

int ref_speed    = 512;
int acceleration = 1;

int main(void)
{
    init();
    __enable_irq();

    MB_WRITE_REG( MB_REG_STATUS, 0x55aa );
    MB_WRITE_REG( MB_REG_CONTROL, 0xaa55 );

    for(;;)
    {
        if (GPIOC->ODR & GPIO_ODR_ODR8)
            GPIOC->ODR &= ~GPIO_ODR_ODR8;

        asm("":::"memory");

        if (sampling_time_flag)
        {
            sampling_time_flag = 0;

            int frequency = open_loop_acceleration(ref_speed, acceleration);

            sinep.direction     = is_negative(frequency);
            sinep.amplitude_pwm = drive_vf_control(abs(frequency));
            sinep.freq_m        = abs(frequency);

            sine_set_params(&sinep);
        }

//        modbus_polling();
        modbus_fsm();
    }

    return 0;
}

