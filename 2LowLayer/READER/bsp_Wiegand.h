/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_Wiegand.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月29日
  最近修改   :
  功能描述   : 韦根读卡器头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月29日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __BSP_WIEGAND_H
#define __BSP_WIEGAND_H


/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "sys.h"
#include "delay.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define WG_TIMEOUT   	4

#if 0

#define WG1_RCC         RCC_AHB1Periph_GPIOE
#define WG1_GPIO_PORT   GPIOE
#define WG1_IN_D0       GPIO_Pin_10//韦根接口
#define WG1_IN_D1       GPIO_Pin_11//韦根接口

#define WG1_PortSource   EXTI_PortSourceGPIOE   

#define WG1_IN_D0_PinSource EXTI_PinSource10
#define WG1_IN_D1_PinSource EXTI_PinSource11


#define WG1_IN_D0_EXTI  EXTI_Line10//韦根接口
#define WG1_IN_D1_EXTI  EXTI_Line11//韦根接口

#else
#define WG1_RCC         RCC_AHB1Periph_GPIOE
#define WG1_GPIO_PORT   GPIOE
#define WG1_IN_D0       GPIO_Pin_12//韦根接口
#define WG1_IN_D1       GPIO_Pin_13//韦根接口

#define WG1_PortSource   EXTI_PortSourceGPIOE   

#define WG1_IN_D0_PinSource EXTI_PinSource12
#define WG1_IN_D1_PinSource EXTI_PinSource13


#define WG1_IN_D0_EXTI  EXTI_Line12//韦根接口
#define WG1_IN_D1_EXTI  EXTI_Line13//韦根接口

#endif

#define WG_IN_D0  GPIO_ReadInputDataBit(WG1_GPIO_PORT,WG1_IN_D0) 
#define WG_IN_D1  GPIO_ReadInputDataBit(WG1_GPIO_PORT,WG1_IN_D1) 




/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
typedef struct
{
  volatile uint8_t WG_Rx_Len; //接收的位长度
  volatile uint8_t WG_Rx_End; //接收结束
  volatile uint8_t WG_Bit;     //位值
  volatile uint8_t WG_Rx_Bit;  //收到位
  volatile uint8_t END_TIME_C_EN;  //接收结束计时使能
  volatile uint32_t WG_Rx_Data;   //接收数据
  volatile uint8_t End_TIME;      //接收结束时间
}WG_RX_T;   //韦根接收结构体


extern WG_RX_T WG_Rx_Str;   //韦根接收结构体



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void WeigenInD0(void);
void WeigenInD1(void);

void bsp_WiegandInit(void);
uint32_t bsp_WeiGenScanf(void);




#endif



