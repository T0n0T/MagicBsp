/**
 * @file printf.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <main.h>
#include <stdio.h>

int fputc(int ch, FILE *stream)
{
    while ((USART1->SR & 0X40) == 0)
        ;

    USART1->DR = (uint8_t)ch;
    return ch;
}
