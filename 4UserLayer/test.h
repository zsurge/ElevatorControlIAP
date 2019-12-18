/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : test.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月28日
  最近修改   :
  功能描述   : 功能模块测试
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月28日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __TEST_H
#define __TEST_H

//标准库头文件
#include "stdlib.h"

#include "cJSON.h"


//中间层头文件
#include "sys.h"
#include "delay.h"
#include "easyflash.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sfud.h"
#include "ini.h"


//驱动层头文件
#include "bsp_time.h"
#include "bsp_led.h"
#include "bsp_key.h"

#include "bsp_spi_flash.h"






#define MY_DEBUGF
#ifdef MY_DEBUGF  
#define MY_DEBUG(message,...) do { \
                                  {UARTprintf(message,##__VA_ARGS__);}\
                               } while(0)  
#else    
#define MY_DEBUG(message,...)    
#endif /* PLC_DEBUG */




void FlashTest(void);
void test_env(void);

//uint8_t SPI_Flash_Test(void);


int CJSON_CDECL json_test(void);
//void spi_flash_demo(void);


void testled(uint8_t flag);






#endif


