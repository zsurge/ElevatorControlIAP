/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_spi_flash.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月26日
  最近修改   :
  功能描述   : SPI接口FLASH读写操作
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月26日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f4xx_conf.h"


/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define SF_MAX_PAGE_SIZE	(4 * 1024)

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
/* 定义串行Flash ID */
enum
{
    SST25VF016B_ID = 0xBF2541,
    MX25L1606E_ID  = 0xC22015,
    W25Q64BV_ID    = 0xEF4017,
    W25Q128FV_ID     = 0XEF4018
};
    



typedef struct
{
    uint32_t ChipID;        /* 芯片ID */
    char ChipName[16];      /* 芯片型号字符串，主要用于显示 */
    uint32_t TotalSize;     /* 总容量 */
    uint16_t PageSize;      /* 页面大小 */
}SFLASH_T;


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
 
 void bsp_spi_flash_init(void);
 uint32_t bsp_sf_ReadID(void);
 void bsp_sf_EraseChip(void);
 void bsp_sf_EraseSector(uint32_t _uiSectorAddr);
 void bsp_sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
 uint8_t bsp_sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
 void bsp_sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
 
 extern SFLASH_T g_tSF;
 
#endif


