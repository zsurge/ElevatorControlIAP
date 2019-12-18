/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : stmflash.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月3日
  最近修改   :
  功能描述   : 片内FLASH操作
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月3日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __STMFLASH_H__
#define __STMFLASH_H__

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "stm32f4xx_flash.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//F407 片内FLASH扇区
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

//片内FLASH结束地址
#define USER_FLASH_END_ADDRESS        0x080FFFFF

//APP大小
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - APPLICATION_ADDRESS + 1)

//APP起始地址    
#define APPLICATION_ADDRESS   (uint32_t)0x08030000 //User Application's location address (sector 4~11)

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/		
void STM_FLASH_Init(void);
uint32_t STM_FLASH_Erase(uint32_t StartSector);
uint32_t STM_FLASH_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength);
uint32_t STM_FLASH_DisableWriteProtection(void);
uint16_t STM_FLASH_GetWriteProtectionStatus(void);			 									   
#endif












