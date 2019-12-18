/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : iap.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月4日
  最近修改   :
  功能描述   : 串口升级程序
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月4日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __IAP_H
#define __IAP_H

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "stm32f4xx.h"
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/


#define QUEUE_MAX_LEN           1024*4 //存储数据元素的最大个数
#define PAGE_SIZE   1024

#define READOVER -2//串口数据超出
#define WRITEOVER  -3//串口写数据越界
#define OK  1



enum upSteps
{
    HTTP_CONNECT = 1,
    HTTP_CONNACK,
    HTTP_SAVE_DATA,
    HTTP_CLOSE
};


typedef struct {
    volatile char data[QUEUE_MAX_LEN];
    volatile int Pwrite; //写指针
    volatile int Pread; //读指针
    volatile int Count; //缓冲区计数
}ReceriveType, *BINQUEUE_T;


extern ReceriveType gBinQueue;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/



/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

uint8_t IAP_JumpToApplication(void);
//void IAP_JumpToApplication(void);

int32_t IAP_DownLoadToFlash(void);

int32_t IAP_DownLoadToSTMFlash(int32_t filesize);


void ClearBinQueue(BINQUEUE_T q);
int ReadBinQueue(BINQUEUE_T queue,uint8_t *buf,uint16_t length);
int WriteBinQueue(BINQUEUE_T queue,uint8_t *buf,uint16_t length);







#endif


