#if 1
#include "main.h"
#include <stdio.h>

/* ��֪����������C������ʹ�ð������ĺ��� */
#pragma import(__use_no_semihosting)

/* ���� _sys_exit() �Ա���ʹ�ð�����ģʽ */
void _sys_exit(int x)
{
    x = x;
}

/* ��׼����Ҫ��֧������ */
struct __FILE
{
    int handle;
};

FILE __stdout;

/*  */
int fputc(int ch, FILE *stream)
{
    /* �����жϴ����Ƿ������ */
    /* ��ͬоƬ�Ĵ��ڱ�־λ��һ����ͬ�� */
    while((USART1->SR & 0X40) == 0);

    /* ���ڷ�����ɣ������ַ����� */
    USART1->DR = (uint8_t) ch;

    return ch;
}

#endif

