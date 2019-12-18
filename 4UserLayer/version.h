
#ifndef __VERSION_H
#define __VERSION_H

#include "stm32f4xx_conf.h"

typedef uint8_t*(*CallBackFun)(void); //获取SN的回调


typedef struct
{
	const uint8_t *SoftwareVersion;       //软件版本号
    const uint8_t *HardwareVersion;  //硬件版本    	
	const uint8_t *BulidDate;         //编译时间
    const uint8_t *Model;        //设备型号
    const uint8_t *ProductBatch; //生产批次
    CallBackFun GetSn; //获取当前设备SN    
}DEVICEINFO_T;

extern const DEVICEINFO_T gDevinfo;

#endif

