/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_flash.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月23日
  最近修改   :
  功能描述   : FLASH驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月23日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#include "bsp_flash.h"

u16 FLASH_TYPE=W25Q128;	//默认是W25Q128

void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_ReadWriteByte(u8 TxData);



//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Init(void)
{	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI1时钟

    //GPIOFB3,4,5初始化设置
    GPIO_InitStructure.GPIO_Pin = FLASH_SCK_PIN|FLASH_MISO_PIN|FLASH_MOSI_PIN;//PB3~5复用功能输出	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(FLASH_PORT, &GPIO_InitStructure);//初始化

    GPIO_PinAFConfig(FLASH_PORT,FLASH_SCK_PINSource,GPIO_AF_SPI1); //PB3复用为 SPI1
    GPIO_PinAFConfig(FLASH_PORT,FLASH_MISO_PINSource,GPIO_AF_SPI1); //PB4复用为 SPI1
    GPIO_PinAFConfig(FLASH_PORT,FLASH_MOSI_PINSource,GPIO_AF_SPI1); //PB5复用为 SPI1

    //这里只针对SPI口初始化
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);//复位SPI1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);//停止复位SPI1

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI1, ENABLE); //使能SPI外设

    SPI1_ReadWriteByte(0xff);//启动传输		
}   

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1 
} 


//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  

    SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
    	
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  

    return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据				    
}


/*****************************************************************************
 函 数 名  : bsp_Flash_Init
 功能描述  : FLASH 初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2019年5月24日
    作    者   : 张舵

    修改内容   : 新生成函数

*****************************************************************************/
void bsp_Flash_Init( void )
{
    //初始化SPI FLASH的IO口
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟

    //GPIOB14
    GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;//PB14
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(FLASH_PORT, &GPIO_InitStructure);//初始化

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//PG7
    GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化

    GPIO_SetBits(GPIOG,GPIO_Pin_7); //PG7输出1,防止NRF干扰SPI FLASH的通信 

    
    FLASH_CS=1;			        //SPI FLASH不选中
    SPI1_Init();		   			//初始化SPI
    SPI1_SetSpeed(SPI_BaudRatePrescaler_4);		//设置为21M时钟,高速模式 
    FLASH_TYPE = bsp_FLASH_ReadID();	            //读取FLASH ID.
}

//读取FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
u8 bsp_FLASH_ReadSR(void)   
{  
	u8 byte=0;   
	FLASH_CS=0;                            //使能器件   
	SPI1_ReadWriteByte(FLASH_ReadStatusReg);	//发送读取状态寄存器命令    
	byte=SPI1_ReadWriteByte(0Xff);        	//读取一个字节  
	FLASH_CS=1;                            //取消片选     
	return byte;   
} 
//写FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void bsp_FLASH_Write_SR(u8 sr)   
{   
	FLASH_CS=0;                            //使能器件   
	SPI1_ReadWriteByte(FLASH_WriteStatusReg);   //发送写取状态寄存器命令    
	SPI1_ReadWriteByte(sr);              	//写入一个字节  
	FLASH_CS=1;                            //取消片选     	      
}   
//FLASH写使能	
//将WEL置位   
void bsp_FLASH_Write_Enable(void)   
{
	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_WriteEnable); 	//发送写使能  
	FLASH_CS=1;                            //取消片选     	      
} 
//FLASH写禁止	
//将WEL清零  
void bsp_FLASH_Write_Disable(void)   
{  
	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_WriteDisable); 	//发送写禁止指令    
	FLASH_CS=1;                            //取消片选     	      
} 		
//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
u16 bsp_FLASH_ReadID(void)
{
	u16 Temp = 0;	  
	FLASH_CS=0;				    
	SPI1_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	FLASH_CS=1;				    
	return Temp;
}   		    
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void bsp_FLASH_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
 	u16 i;   										    
	FLASH_CS=0;                            	//使能器件   
    SPI1_ReadWriteByte(FLASH_ReadData);   		//发送读取命令   
    SPI1_ReadWriteByte((u8)((ReadAddr)>>16));	//发送24bit地址    
    SPI1_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI1_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPI1_ReadWriteByte(0XFF);   	//循环读数  
    }
	FLASH_CS=1;  				    	      
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void bsp_FLASH_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
    bsp_FLASH_Write_Enable();                  //SET WEL 
	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_PageProgram);      //发送写页命令   
    SPI1_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数  
	FLASH_CS=1;                 			//取消片选 
	bsp_FLASH_Wait_Busy();						//等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void bsp_FLASH_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		bsp_FLASH_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535) 
#if FLASH_USE_MALLOC==0 
u8 FLASH_BUFFER[4096];		 
#endif
void bsp_FLASH_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * FLASH_BUF;	
#if	FLASH_USE_MALLOC==1	//动态内存管理
	FLASH_BUF=mymalloc(SRAMIN,4096);//申请内存
#else
   	FLASH_BUF=FLASH_BUFFER; 
#endif     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		bsp_FLASH_Read(FLASH_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			bsp_FLASH_Erase_Sector(secpos);	//擦除这个扇区
			for(i=0;i<secremain;i++)	  	//复制
			{
				FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			bsp_FLASH_Write_NoCheck(FLASH_BUF,secpos*4096,4096);//写入整个扇区  

		}else bsp_FLASH_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	
#if	FLASH_USE_MALLOC==1		 
	myfree(SRAMIN,FLASH_BUF);	//释放内存
#endif	
}
//擦除整个芯片		  
//等待时间超长...
void bsp_FLASH_Erase_Chip(void)   
{                                   
    bsp_FLASH_Write_Enable();                  //SET WEL 
    bsp_FLASH_Wait_Busy();   
  	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_ChipErase);		//发送片擦除命令  
	FLASH_CS=1;                            //取消片选     	      
	bsp_FLASH_Wait_Busy();   				   	//等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void bsp_FLASH_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用    	
 	Dst_Addr *= 4096;
    bsp_FLASH_Write_Enable();                  //SET WEL 	 
    bsp_FLASH_Wait_Busy();   
  	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_SectorErase);  	//发送扇区擦除指令 
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
    SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI1_ReadWriteByte((u8)Dst_Addr);  
	FLASH_CS=1;                       		//取消片选     	      
    bsp_FLASH_Wait_Busy();   				   	//等待擦除完成
}  
//等待空闲
void bsp_FLASH_Wait_Busy(void)   
{   
	while((bsp_FLASH_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void bsp_FLASH_PowerDown(void)   
{ 
  	FLASH_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(FLASH_PowerDown);     //发送掉电命令  
	FLASH_CS=1;                            //取消片选     	      
    delay_us(3);                            //等待TPD  
}   
//唤醒
void bsp_FLASH_WAKEUP(void)   
{  
  	FLASH_CS=0;                            	//使能器件   
    SPI1_ReadWriteByte(FLASH_ReleasePowerDown);	//  send FLASH_PowerDown command 0xAB    
	FLASH_CS=1;                            	//取消片选     	      
    delay_us(3);                               	//等待TRES1
}   





