/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : stmflash.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月3日
  最近修改   :
  功能描述   : 芯片内部FLASH操作
  函数列表   :
              GetSector     //根据地址返回扇区号
              STM_FLASH_DisableWriteProtection  //
              STM_FLASH_Erase
              STM_FLASH_GetWriteProtectionStatus
              STM_FLASH_Init
              STM_FLASH_Write
  修改历史   :
  1.日    期   : 2019年6月3日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "stmflash.h"


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
 
//Private Function
static uint32_t GetSector(uint32_t Address);

/*****************************************************************************
 函 数 名  : STM_FLASH_Init
 功能描述  : 片内FLASH初始化，解锁
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵

    修改内容   : 新生成函数

*****************************************************************************/
void STM_FLASH_Init(void)
{ 
  FLASH_Unlock(); 

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}

/*****************************************************************************
 函 数 名  : STM_FLASH_Erase
 功能描述  : 擦除所有用户的flash区域
 输入参数  : StartSector 用户APP起始地址
 输出参数  : 无
 返 回 值  : 0:擦除成功
             1:擦除错误
 
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵

    修改内容   : 新生成函数

*****************************************************************************/  
uint32_t STM_FLASH_Erase(uint32_t StartSector)
{
  uint32_t UserStartSector = FLASH_Sector_1, i = 0;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(StartSector);

  for(i = UserStartSector; i <= FLASH_Sector_11; i += 8)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
    {
      /* Error occurred while page erase */
      return (1);
    }
  }
  
  return (0);
}

//This function writes a data buffer in flash (data are 32-bit aligned).
//After writing data buffer, the flash content is checked.
//FlashAddress: start address for writing data buffer
//Data: pointer on data buffer
//DataLength: length of data buffer (unit is 32-bit word)   
//return:
//      0=Data successfully written to Flash memory
//      1=Error occurred while writing data in Flash memory
//      2=Written Data in flash memory is different from expected one



/*****************************************************************************
 函 数 名  : STM_FLASH_Write
 功能描述  : FLASH写入操作(数据是32位对齐的)
 输入参数  : __IO uint32_t* FlashAddress   要写入的起始地址
             uint32_t* Data              要写入的数据指针
             uint32_t DataLength         要写入数据的长度(unit is 32-bit word)   
 输出参数  : 无
 返 回 值  : 0=数据写入成功
             1=写入数据时发生错误
             2=写入的数据与预期的不同
 
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵

    修改内容   : 新生成函数

*****************************************************************************/
uint32_t STM_FLASH_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
  uint32_t i = 0;

  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
    {
     /* Check the written value */
      if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
      {
        /* Flash content doesn't match SRAM content */
        return(2);
      }
      /* Increment FLASH destination address */
      *FlashAddress += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
      return (1);
    }
  }
  return (0);
}




/*****************************************************************************
 函 数 名  : STM_FLASH_GetWriteProtectionStatus
 功能描述  : 返回用户flash区域的写保护状态
 输入参数  : void
 输出参数  : 无
 返 回 值  : 0=用户flash区域内没有写保护扇区
             1=用户flash区域内的某些扇区受写保护
            
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵

    修改内容   : 新生成函数

*****************************************************************************/
uint16_t STM_FLASH_GetWriteProtectionStatus(void)
{
  uint32_t UserStartSector = FLASH_Sector_1;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(APPLICATION_ADDRESS);

  /* Check if there are write protected sectors inside the user flash area */
  if ((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8)))
  { /* No write protected sectors inside the user flash area */
    return 1;
  }
  else
  { /* Some sectors inside the user flash area are write protected */
    return 0;
  }
}

/*****************************************************************************
 函 数 名  : STM_FLASH_DisableWriteProtection
 功能描述  : 禁用用户flash区域的写保护。
 输入参数  : void  
 输出参数  : 
 返 回 值  :    1:写保护成功禁用
             2:错误:Flash写未保护失败
 
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵
    修改内容   : 新生成函数

*****************************************************************************/
uint32_t STM_FLASH_DisableWriteProtection(void)
{
  __IO uint32_t UserStartSector = FLASH_Sector_1, UserWrpSectors = OB_WRP_Sector_1;

  /* Get the sector where start the user flash area */
  UserStartSector = GetSector(APPLICATION_ADDRESS);

  /* Mark all sectors inside the user flash area as non protected */  
  UserWrpSectors = 0xFFF-((1 << (UserStartSector/8))-1);
   
  /* Unlock the Option Bytes */
  FLASH_OB_Unlock();

  /* Disable the write protection for all sectors inside the user flash area */
  FLASH_OB_WRPConfig(UserWrpSectors, DISABLE);

  /* Start the Option Bytes programming process. */  
  if (FLASH_OB_Launch() != FLASH_COMPLETE)
  {
    /* Error: Flash write unprotection failed */
    return (2);
  }

  /* Write Protection successfully disabled */
  return (1);
}


/*****************************************************************************
 函 数 名  : GetSector
 功能描述  : 获取指定地址的扇区号
 输入参数  : uint32_t Address  
 输出参数  : 
 返 回 值  : 扇区号
 
 修改历史      :
  1.日    期   : 2019年6月3日
    作    者   : 张舵   
    修改内容   : 新生成函数

*****************************************************************************/
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_Sector_11;  
  }
    return sector;
}







