/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_DigitalTube.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月2日
  最近修改   :
  功能描述   : 驱动74HC595数码管
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月2日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __DIGITALTUBE_H
#define __DIGITALTUBE_H

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "delay.h"
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define HC595_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define HC595_GPIO_PORT         GPIOA

#define	HC595_CLK_PIN    	    GPIO_Pin_6
#define	HC595_CS_PIN    	    GPIO_Pin_5
#define	HC595_DATA_PIN    	    GPIO_Pin_4



#define HC595_CLK_H()           HC595_GPIO_PORT->BSRRH = HC595_CLK_PIN
#define HC595_CLK_L()		    HC595_GPIO_PORT->BSRRL  = HC595_CLK_PIN

#define HC595_CS_H()            HC595_GPIO_PORT->BSRRH = HC595_CS_PIN
#define HC595_CS_L()		    HC595_GPIO_PORT->BSRRL = HC595_CS_PIN

#define HC595_DATA_H()          HC595_GPIO_PORT->BSRRH = HC595_DATA_PIN
#define HC595_DATA_L()		    HC595_GPIO_PORT->BSRRL = HC595_DATA_PIN


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
//共阳：
//char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};
//共阴：
//char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

void bsp_HC595Init(void);

void bsp_HC595Send(uint16_t data);
void bsp_HC595Load(void);
void bsp_HC595Show(uint8_t bit1, uint8_t bit2,uint8_t bit3);
void bsp_HC595RowOut(uint32_t Data);


#endif



