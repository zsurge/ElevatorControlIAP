/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_Wiegand.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月29日
  最近修改   :
  功能描述   : 韦根读卡器驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月29日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "bsp_Wiegand.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

static uint8_t WG_Rx_Pro ( void );

static uint8_t ECC_ODD_Check ( uint32_t EvevData,uint8_t Bitlen );
static uint8_t ECC_Even_Check ( uint32_t EvevData,uint8_t Bitlen );


WG_RX_T WG_Rx_Str;   //韦根接收结构体


static uint8_t g_WGCardId[4] = {0};
static uint8_t CheckBitStart = 0;


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/


void bsp_WiegandInit ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_AHB1PeriphClockCmd ( WG1_RCC, ENABLE ); //GPIOD时钟使能

	//GPIO
	GPIO_InitStructure.GPIO_Pin = WG1_IN_D0 | WG1_IN_D1;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    //管脚速率选择
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		//管脚功能配置
	GPIO_Init ( WG1_GPIO_PORT, &GPIO_InitStructure );   //端口选择：如PA，PB口

//    GPIO_SetBits(WG1_GPIO_PORT,WG1_IN_D0);
//    GPIO_SetBits(WG1_GPIO_PORT,WG1_IN_D1);

	/* EXTI line gpio config(PD10 PD11) */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_SYSCFG, ENABLE ); //使能SYSCFG时钟

	/* IO3_1 连接 EXTI Line10 到 PD10 引脚 */
	SYSCFG_EXTILineConfig ( WG1_PortSource, WG1_IN_D0_PinSource );

	/* IO3_1连接 EXTI Line11 到 PD11 引脚 */
	SYSCFG_EXTILineConfig ( WG1_PortSource, WG1_IN_D1_PinSource );


	EXTI_InitStructure.EXTI_Line = WG1_IN_D0_EXTI | WG1_IN_D1_EXTI;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init ( &EXTI_InitStructure );

	/* 配置PD10 11为中断源 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init ( &NVIC_InitStructure );
}


void EXTI15_10_IRQHandler ( void )
{

	if ( EXTI_GetITStatus ( WG1_IN_D0_EXTI ) != RESET ) //D0
	{
		//韦根取D0管脚数据
        delay_us(30);//消抖
        if(WG_IN_D0==0)     
        {                
            WeigenInD0();
        }

		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit ( WG1_IN_D0_EXTI );
	}



	if ( EXTI_GetITStatus ( WG1_IN_D1_EXTI ) != RESET ) //D1
	{
		//韦根取D1管脚数据		
        delay_us(30);//消抖
        
        if(WG_IN_D1==0)     
        {                
            WeigenInD1();
        }

		/* Clear the EXTI line 1 pending bit */
		EXTI_ClearITPendingBit ( WG1_IN_D1_EXTI );
	}

}



void WeigenInD0 ( void)
{
	WG_Rx_Str.WG_Bit=0;

	WG_Rx_Str.End_TIME=WG_TIMEOUT;   //40~50ms

	if ( WG_Rx_Str.WG_Rx_Len<34 )
	{
		WG_Rx_Str.WG_Rx_Len++;
	}

	else if ( WG_Rx_Str.WG_Rx_Len==34 )
	{
		WG_Rx_Str.WG_Rx_End=1;
		WG_Rx_Str.END_TIME_C_EN=0;
	}
	else
	{
		WG_Rx_Str.WG_Rx_Len=0;
		WG_Rx_Str.WG_Rx_End=0;
		WG_Rx_Str.END_TIME_C_EN=0;
	}

	if ( WG_Rx_Str.WG_Rx_Len==1 )
	{
		WG_Rx_Str.WG_Rx_Data=0;
		WG_Rx_Str.END_TIME_C_EN=1;
		CheckBitStart = WG_Rx_Str.WG_Bit;  //2012-10-13 ,增加奇偶效验位;
	}

	if ( ( 1<WG_Rx_Str.WG_Rx_Len ) && ( WG_Rx_Str.WG_Rx_Len<34 ) ) 	//2012-09-19 Leo 修改，解决Wiegand接收数据判定长度有误的bug
	{
		if ( WG_Rx_Str.WG_Bit )
		{
			WG_Rx_Str.WG_Rx_Data|=0x01;
		}
		else
		{
			WG_Rx_Str.WG_Rx_Data&=0xFFFFFFFE;
		}
		if ( WG_Rx_Str.WG_Rx_Len<33 )
		{
			WG_Rx_Str.WG_Rx_Data<<=1;
		}
	}
}


void WeigenInD1 ( void )
{
	WG_Rx_Str.WG_Bit=1;
	WG_Rx_Str.End_TIME=WG_TIMEOUT;   //40~50ms
	if ( WG_Rx_Str.WG_Rx_Len<34 )
	{
		WG_Rx_Str.WG_Rx_Len++;
	}
	else if ( WG_Rx_Str.WG_Rx_Len==34 )
	{
		WG_Rx_Str.WG_Rx_End=1;
		WG_Rx_Str.END_TIME_C_EN=0;

	}
	else
	{
		WG_Rx_Str.WG_Rx_Len=0;
		WG_Rx_Str.WG_Rx_End=0;
		WG_Rx_Str.END_TIME_C_EN=0;
	}
	if ( WG_Rx_Str.WG_Rx_Len==1 )
	{
		WG_Rx_Str.WG_Rx_Data=0;
		WG_Rx_Str.END_TIME_C_EN=1;
		CheckBitStart = WG_Rx_Str.WG_Bit; ;  //2012-10-13 ,增加奇偶效验位;
	}

	if ( ( 1<WG_Rx_Str.WG_Rx_Len ) && ( WG_Rx_Str.WG_Rx_Len<34 ) )		//2012-09-19 Leo 修改，解决Wiegand接收数据判定长度有误的bug
	{
		if ( WG_Rx_Str.WG_Bit )
		{
			WG_Rx_Str.WG_Rx_Data|=0x01;
		}
		else
		{
			WG_Rx_Str.WG_Rx_Data&=0xFFFFFFFE;
		}
		if ( WG_Rx_Str.WG_Rx_Len<33 )
		{
			WG_Rx_Str.WG_Rx_Data<<=1;
		}
	}
}


uint8_t WG_Rx_Pro ( void )
{
	uint8_t wg_len=0;
	uint8_t WGBit;

	wg_len=WG_Rx_Str.WG_Rx_Len;


	WG_Rx_Str.WG_Rx_Len=0;
	if ( wg_len!=26&&wg_len!=34 )
	{
		return 0;
	}
	if ( wg_len==34 ) //wg34
	{
		g_WGCardId[0] = ( WG_Rx_Str.WG_Rx_Data&0xFF000000 ) >>24;	//wg34
	}
	else //wg26
	{
		g_WGCardId[0]=0;
		WGBit = ( WG_Rx_Str.WG_Rx_Data>>1 ) &0x01;
		if ( ECC_ODD_Check ( WG_Rx_Str.WG_Rx_Data>>2,12 ) != WGBit )
		{
			return 0;
		}

		WG_Rx_Str.WG_Rx_Data>>=2;
		if ( ECC_Even_Check ( WG_Rx_Str.WG_Rx_Data>>12,12 ) != CheckBitStart )
		{
			return 0;
		}
	}


	g_WGCardId[1]= ( WG_Rx_Str.WG_Rx_Data&0xFF0000 ) >>16;
	g_WGCardId[2]= ( WG_Rx_Str.WG_Rx_Data&0xFF00 ) >>8;
	g_WGCardId[3]=WG_Rx_Str.WG_Rx_Data&0xFF;
	WG_Rx_Str.WG_Rx_Data=0;
	return wg_len;
}


uint32_t bsp_WeiGenScanf (void)
{
	uint8_t len=0;
	uint32_t Serial_Number = 0;
	/*wiegand处理*/

	if ( WG_Rx_Str.END_TIME_C_EN )
	{
		if ( !WG_Rx_Str.End_TIME )
		{
			WG_Rx_Str.END_TIME_C_EN=0;
			if ( ( WG_Rx_Str.WG_Rx_Len==26 ) || ( WG_Rx_Str.WG_Rx_Len==34 ) )
			{
				WG_Rx_Str.WG_Rx_End=1;
			}
			else
			{
				WG_Rx_Str.WG_Rx_End=0;
				WG_Rx_Str.WG_Rx_Len = 0;
				WG_Rx_Str.End_TIME = WG_TIMEOUT;
			}
		}
	}
	else
	{
		if ( !WG_Rx_Str.End_TIME )
		{
			WG_Rx_Str.WG_Rx_Len=0;
			WG_Rx_Str.WG_Rx_End=0;
			WG_Rx_Str.End_TIME = WG_TIMEOUT;
		}
	}

	if ( WG_Rx_Str.WG_Rx_End )
	{
		WG_Rx_Str.WG_Rx_End=0;

		len = WG_Rx_Pro();

		if ( len==34 )
		{
			Serial_Number |= g_WGCardId[0]<<24;
			Serial_Number |= g_WGCardId[1]<<16;
			Serial_Number |= g_WGCardId[2]<<8;
			Serial_Number |= g_WGCardId[3];

			g_WGCardId[0] = 0;
			g_WGCardId[1] = 0;
			g_WGCardId[2] = 0;
			g_WGCardId[3] = 0;
		}
		else if ( len==26 )
		{

			Serial_Number |= g_WGCardId[1]<<16;
			Serial_Number |= g_WGCardId[2]<<8;
			Serial_Number |= g_WGCardId[3];

			if ( ( g_WGCardId[1] == 0xFF ) && ( g_WGCardId[2] == 0xFF ) )
			{
				if ( g_WGCardId[3] >= 0xF0 )
				{
					len = 4;
				}
				else
				{
                    len = 8;
				}
			}

			g_WGCardId[0] = 0;
			g_WGCardId[1] = 0;
			g_WGCardId[2] = 0;
			g_WGCardId[3] = 0;
		}

	}
    


	return Serial_Number;
}

uint8_t ECC_Even_Check ( uint32_t EvevData,uint8_t Bitlen )
{
	uint8_t i,Num=0;
	uint32_t Data,EccEven = 0x00000001;

	for ( i=0; i<Bitlen; i++ )
	{
		Data = EccEven&EvevData;
		if ( Data==1 )
		{
			Num ++;
		}
		EvevData>>=1;
	}
	Num = Num%2;

	if ( Num )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


uint8_t ECC_ODD_Check ( uint32_t EvevData,uint8_t Bitlen )
{
	uint8_t i,Num=0;
	uint32_t Data,EccEven = 0x00000001;

	for ( i=0; i<Bitlen; i++ )
	{
		Data = EccEven&EvevData;
		if ( Data==1 )
		{
			Num ++;
		}
		EvevData>>=1;
	}
	Num = Num%2;

	if ( Num )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


