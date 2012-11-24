#include "crc32.h"
#include <stdint.h>
#include <stm32f10x.h>

uint32_t bit_reverse(uint32_t data)
{
    asm volatile ("rbit  %0, %1 \n\t" : "=r" (data) : "r" (data));
    return data;
}

uint32_t crc32(uint8_t pBuffer[], uint32_t bufLength)
{
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    CRC->CR     |= CRC_CR_RESET;

    asm( "nop" );
    asm( "nop" );
    asm( "nop" );
    asm( "nop" );

    uint32_t bufLenInWord = bufLength >> 2;
    uint32_t temp;
    
    while (bufLenInWord--) {
        temp = *((uint32_t*) pBuffer);
        CRC->DR = bit_reverse(temp);
        pBuffer += 4;
    }

    temp = bit_reverse(CRC->DR);
    
    uint32_t remainingBytes = bufLength & 0x03;

    while (remainingBytes--) {
        temp ^= (uint32_t) *(pBuffer++);

        for (int i = 0; i < 8; i++) {
            if (temp & 0x01) {
                temp = (temp >> 1) ^ 0xEDB88320;
            } else {
                temp >>= 1;
            }
        }
    }

    temp ^= 0xffffffff;
    return temp;
}

