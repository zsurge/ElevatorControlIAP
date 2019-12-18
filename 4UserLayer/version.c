/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : Version.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年8月21日
  最近修改   :
  功能描述   : 实现版本管理
  函数列表   :
  修改历史   :
  1.日    期   : 2019年8月21日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "version.h" 
#include "tool.h"
#include "string.h"
#include "stdio.h"
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
static uint8_t * GetDevSn(void);
const uint8_t VersionString[] = "1.0.1";
const uint8_t Hardversion[] = "1.0.1";
const uint8_t SoftBulidDate[] = __DATE__ " " __TIME__;
const uint8_t DevModel[] = "ElevatorControlIAP";
const uint8_t DevBatch[] = "20191212001";


static uint8_t * GetDevSn(void)
{ 
    static unsigned int chipid[3] = { 0 };
    
    static unsigned char id[30] = {0};
    
    static unsigned char id1[10] = { 0 };
    static unsigned char id2[10] = { 0 };
    static unsigned char id3[10] = { 0 };

    chipid[0] = *(__I uint32_t *)(0x1FFF7A10 + 0x00);
    chipid[1] = *(__I uint32_t *)(0x1FFF7A10 + 0x04);
    chipid[2] = *(__I uint32_t *)(0x1FFF7A10 + 0x08); 

    int2Str(id1, chipid[0]);
	int2Str(id2, chipid[1]);
	int2Str(id3, chipid[2]);

    strcpy((char *)id,(char *)id1);
    strcat((char *)id,(char *)id2);
    strcat((char *)id,(char *)id3);

    return id;  
}

const DEVICEINFO_T gDevinfo = {VersionString, Hardversion,SoftBulidDate,DevModel,DevBatch,GetDevSn};