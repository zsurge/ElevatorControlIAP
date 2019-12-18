/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月9日
  最近修改   :
  功能描述   : 这是底层驱动模块所有的h文件的汇总文件。
            应用程序只需 #include bsp.h 即可，不需要#include 每个模块的 h 文件
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月9日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __BSP_H
#define __BSP_H
/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
//标准库头文件
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

//中间件
#include "stmflash.h"
#include "easyflash.h"
//#include "sfud.h"

#include "lwip_comm.h"
#include "lwip/netif.h"
#include "lwipopts.h"




//驱动层头文件
#include "bsp_flash.h"
#include "bsp_uart_fifo.h"

#include "LAN8720.h"

//#include "bsp_usart.h"
//#include "bsp_usart1.h"
//#include "bsp_usart2.h"

//#include "bsp_usart3.h"
//#include "bsp_usart4.h"
//#include "bsp_usart5.h"
#include "bsp_key.h"
#include "bsp_time.h"
#include "bsp_led.h"
#include "bsp_beep.h" 
#include "bsp_spi_flash.h"

//#include "bsp_digitaltube.h"
#include "bsp_iwdg.h"
//#include "bsp_infrared_it.h"
#include "bsp_wiegand.h"
#include "bsp_tim_pwm.h"






/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void bsp_Init(void);



#endif




