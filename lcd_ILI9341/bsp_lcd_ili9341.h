#ifndef __BSP__LCD_ILI93XX_H
#define __BSP__LCD_ILI93XX_H
/**==================================================================================================================
 **【文件名称】  bsp_lcd_2.8_ili93xx.h
 **【功能测试】  显示屏
 **==================================================================================================================
 **【适用平台】  STM32F103 + KEIL5.27 + 2.8寸显示屏_ILI9341
 **
 **【实验操作】
 **
 **
 **【文件移植】
 **
 **【更新记录】  2022-05-22  修改引脚配置代码，使用标准库
 **              2020-12-12  完善修改中英文显示函数
 **              2020-09     创建
 **
 **【备注说明】  代码版权归魔女科技所有，请勿商用，谢谢！
 **              https://demoboard.taobao.com
====================================================================================================================*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>


/*****************************************************************************
 ** 移植配置
****************************************************************************/
// BL_背光  
#define BL  GET_PIN(A, 15)
#define    LCD_BL_GPIO     GPIOA              
#define    LCD_BL_PIN      GPIO_PIN_15
// RD_选读
#define RD  GET_PIN(C, 6)
#define    LCD_RD_GPIO     GPIOC              
#define    LCD_RD_PIN      GPIO_PIN_6
// WR_选写
#define WR  GET_PIN(C, 7)
#define    LCD_WR_GPIO     GPIOC
#define    LCD_WR_PIN      GPIO_PIN_7
// RS_寄存器与数据
#define RS  GET_PIN(C, 8)
#define    LCD_RS_GPIO     GPIOC             
#define    LCD_RS_PIN      GPIO_PIN_8
// CS_片选
#define CS  GET_PIN(C, 9)
#define    LCD_CS_GPIO     GPIOC              
#define    LCD_CS_PIN      GPIO_PIN_9

// 屏幕参数
#define    LCD_WIDTH       240             // 屏幕宽度像素，注意：0~239
#define    LCD_HIGH        320             // 屏幕高度像素，注意：0~319
#define    LCD_DIR         0               // 四种显示方向，0-正竖屏，3-倒竖屏，5-正横屏, 6-倒横屏

#define GBK_STORAGE_ADDR 0x00A00000
//  结束 移植修改 ************************************************************



typedef struct          // 结构体：用于保存信息、状态
{
    rt_uint8_t  FlagInit;       // 初始化完成标志
    rt_uint16_t width;     // LCD 宽度
    rt_uint16_t height;    // LCD 高度
    rt_uint8_t  dir;            // 横屏还是竖屏控制：0，竖屏；1，横屏
    rt_uint16_t id;
} _LCD;
extern _LCD xLCD;



/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define      WHITE               0xFFFF       // 白色
#define      BLACK               0x0000       // 黑色 
#define      GREY                0xF7DE       // 灰色 
#define      RED                 0xF800       // 红 
#define      MAGENTA             0xF81F       // 洋红色 
#define      GRED                0xFFE0       // 深红色
#define      GREEN               0x07E0       // 绿 
#define      CYAN                0x7FFF       // 青色 
#define      YELLOW              0xFFE0       // 黄色 
#define      LIGHTGREEN          0X841F       // 浅绿色 
#define      BLUE                0x001F       // 蓝 
#define      GBLUE               0x07FF       // 浅蓝 1
#define      LIGHTBLUE           0X7D7C       // 浅蓝 2
#define      BLUE2               0x051F       // 浅蓝 3
#define      GRAYBLUE            0X5458       // 灰蓝 
#define      DARKBLUE            0X01CF       // 深蓝

#define      LGRAY               0XC618       // 浅灰色,窗体背景色
#define      LGRAYBLUE           0XA651       // 浅灰蓝色(中间层颜色)
#define      LBBLUE              0X2B12       // 浅棕蓝色(选择条目的反色)



/*****************************************************************************
 ** 声明全局函数

****************************************************************************/
void LCD_Init(void);                                                                             // 初始化
void LCD_DisplayOn(void);                                                                        // 开显示
void LCD_DisplayOff(void);                                                                       // 关显示
void LCD_DisplayDir(rt_uint8_t dir);                                                                     // 显示方向
void LCD_DrawPoint(rt_uint16_t x, rt_uint16_t y, rt_uint16_t color);                                      // 画点
void LCD_Circle(rt_uint16_t x0, rt_uint16_t y0, rt_uint8_t r, rt_uint16_t color);                                 // 画圆
void LCD_Line(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);               // 画线
void LCD_Cross(rt_uint16_t x, rt_uint16_t y, rt_uint16_t len, rt_uint32_t fColor);                           // 画十字线
void LCD_Rectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2, rt_uint16_t color);          // 画矩形
void LCD_Fill(rt_uint16_t sx, rt_uint16_t sy, rt_uint16_t ex, rt_uint16_t ey, rt_uint16_t color);               // 填充单色
void LCD_String(rt_uint16_t x, rt_uint16_t y, char *pFont, rt_uint8_t size, rt_uint32_t fColor, rt_uint32_t bColor); // 显示中英文字符串
void LCD_Image(rt_uint16_t x, rt_uint16_t y, rt_uint16_t width, rt_uint16_t height, const rt_uint8_t *image) ;       // 显示图像
void LCD_GUI(void);                                                                              // 绘制简单界面

#endif

