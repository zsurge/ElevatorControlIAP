/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : ini.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月28日
  最近修改   :
  功能描述   : 对参数进行操作
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月28日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __INI_H
#define __INI_H

#include "sfud.h"
#include "tool.h"
#include "easyflash.h"
#include "errorcode.h"

typedef struct
{
    uint8_t FunState;
    
    SYSERRORCODE_E (*SaveICParam)(void); //保存参数
}ICREADER_T;

typedef struct
{
    uint8_t FunState;
    uint8_t LightMode;
    uint8_t ScanMode; 
    uint8_t TimeInterval; 
    
    SYSERRORCODE_E (*SaveQRParam)(void); //保存参数   
}QRCODE_T;


extern ICREADER_T   gICReaderParam; 
extern QRCODE_T     gQRCodeParam;

void ReadIAP(void);

void RestoreDefaultSetting(void);

void SystemUpdate(void);

SYSERRORCODE_E RecordBootTimes(void);
SYSERRORCODE_E ParseDevParam(uint8_t *ParamBuff);







#endif
