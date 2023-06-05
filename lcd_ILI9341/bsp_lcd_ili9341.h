#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H
/**==================================================================================================================
 **���ļ����ơ�  bsp_lcd_2.8_ili93xx.h
 **�����ܲ��ԡ�  ��ʾ��
 **==================================================================================================================
 **������ƽ̨��  STM32F103 + KEIL5.27 + 2.8����ʾ��_ILI9341
 **
 **��ʵ�������
 **
 **
 **���ļ���ֲ��
 **
 **�����¼�¼��  2022-05-22  �޸��������ô��룬ʹ�ñ�׼��
 **              2020-12-12  �����޸���Ӣ����ʾ����
 **              2020-09     ����
 **
 **����ע˵����  �����Ȩ��ħŮ�Ƽ����У��������ã�лл��
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>


/*****************************************************************************
 ** ��ֲ����
****************************************************************************/
// BL_����  
#define BL  GET_PIN(A, 15)
#define    LCD_BL_GPIO     GPIOA              
#define    LCD_BL_PIN      GPIO_PIN_15
// RD_ѡ��
#define RD  GET_PIN(C, 6)
#define    LCD_RD_GPIO     GPIOC              
#define    LCD_RD_PIN      GPIO_PIN_6
// WR_ѡд
#define WR  GET_PIN(C, 7)
#define    LCD_WR_GPIO     GPIOC
#define    LCD_WR_PIN      GPIO_PIN_7
// RS_�Ĵ���������
#define RS  GET_PIN(C, 8)
#define    LCD_RS_GPIO     GPIOC             
#define    LCD_RS_PIN      GPIO_PIN_8
// CS_Ƭѡ
#define CS  GET_PIN(C, 9)
#define    LCD_CS_GPIO     GPIOC              
#define    LCD_CS_PIN      GPIO_PIN_9

// ��Ļ����
#define    LCD_WIDTH       240             // ��Ļ�������أ�ע�⣺0~239
#define    LCD_HIGH        320             // ��Ļ�߶����أ�ע�⣺0~319
#define    LCD_DIR         0               // ������ʾ����0-��������3-��������5-������, 6-������

#define GBK_STORAGE_ADDR 0x00A00000
//  ���� ��ֲ�޸� ************************************************************



typedef struct          // �ṹ�壺���ڱ�����Ϣ��״̬
{
    rt_uint8_t  FlagInit;       // ��ʼ����ɱ�־
    rt_uint16_t width;     // LCD ����
    rt_uint16_t height;    // LCD �߶�
    rt_uint8_t  dir;            // ���������������ƣ�0��������1������
    rt_uint16_t id;
} _LCD;
extern _LCD xLCD;



/******************************* ���� ILI934 ��ʾ��������ɫ ********************************/
#define      WHITE               0xFFFF       // ��ɫ
#define      BLACK               0x0000       // ��ɫ 
#define      GREY                0xF7DE       // ��ɫ 
#define      RED                 0xF800       // �� 
#define      MAGENTA             0xF81F       // ���ɫ 
#define      GRED                0xFFE0       // ���ɫ
#define      GREEN               0x07E0       // �� 
#define      CYAN                0x7FFF       // ��ɫ 
#define      YELLOW              0xFFE0       // ��ɫ 
#define      LIGHTGREEN          0X841F       // ǳ��ɫ 
#define      BLUE                0x001F       // �� 
#define      GBLUE               0x07FF       // ǳ�� 1
#define      LIGHTBLUE           0X7D7C       // ǳ�� 2
#define      BLUE2               0x051F       // ǳ�� 3
#define      GRAYBLUE            0X5458       // ���� 
#define      DARKBLUE            0X01CF       // ����

#define      LGRAY               0XC618       // ǳ��ɫ,���屳��ɫ
#define      LGRAYBLUE           0XA651       // ǳ����ɫ(�м����ɫ)
#define      LBBLUE              0X2B12       // ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)



/*****************************************************************************
 ** ����ȫ�ֺ���

****************************************************************************/
void LCD_Init(void);                                                                             // ��ʼ��
void LCD_DisplayOn(void);                                                                        // ����ʾ
void LCD_DisplayOff(void);                                                                       // ����ʾ
void LCD_DisplayDir(rt_uint8_t dir);                                                                     // ��ʾ����
void LCD_DrawPoint(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);                                      // ����
void LCD_Circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r, rt_uint16_t color);                                 // ��Բ
void LCD_Line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);               // ����
void LCD_Cross(rt_uint16_t x, rt_uint16_t y, rt_uint16_t len, rt_uint32_t fColor);                           // ��ʮ����
void LCD_Rectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);          // ������
void LCD_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t color);               // ��䵥ɫ
void LCD_String(rt_uint16_t x, rt_uint16_t y, char *pFont, rt_uint8_t size, rt_uint32_t fColor, rt_uint32_t bColor); // ��ʾ��Ӣ���ַ���
void LCD_Image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, const rt_uint8_t *image) ;       // ��ʾͼ��
void LCD_GUI(void);                                                                              // ���Ƽ򵥽���

#endif
