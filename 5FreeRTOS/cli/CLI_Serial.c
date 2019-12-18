/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/

/* Demo application includes. */
#include "CLISerial.h"


/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE				( ( QueueHandle_t ) 0 )
#define serNO_BLOCK						( ( TickType_t ) 0 )
#define serTX_BLOCK_TIME				( 40 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

/* UART interrupt handler. */
void vUARTInterruptHandler( void );

/*-----------------------------------------------------------*/



void xUsart2Init (uint32_t BaudRate)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

     RCC_AHB1PeriphClockCmd(USART2_GPIO_CLK,ENABLE); //使能GPIOA时钟
     RCC_APB1PeriphClockCmd(USART2_CLK,ENABLE);//使能Usart2时钟

     //串口1对应引脚复用映射
     GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_TX_SOURCE,USART2_TX_AF); //GPIOA2复用为USART2
     GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_RX_SOURCE,USART2_RX_AF); //GPIOA3复用为USART2
     
     //Usart2端口配置
     GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN | USART2_RX_PIN; //GPIOA9与GPIOA10
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //速度50MHz
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
     GPIO_Init(USART2_GPIO_PORT,&GPIO_InitStructure); //初始化PA2，PA3

    //Usart2 初始化设置
     USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
     USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
     USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
     USART_Init(USART2, &USART_InitStructure); //初始化串口2    

     //Usart2 NVIC 配置
     NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;//抢占优先级3
     NVIC_InitStructure.NVIC_IRQChannelSubPriority =5;       //子优先级3
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
     NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
     
     USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断    
     
     USART_Cmd(USART2, ENABLE);  //使能串口1     
}



/*
 * See the serial2.h header file.
 */
xComPortHandle xSerialPortInitMinimal( unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
xComPortHandle xReturn;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

	/* Create the queues used to hold Rx/Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	
	/* If the queue/semaphore was created correctly then setup the serial port
	hardware. */
	if( ( xRxedChars != serINVALID_QUEUE ) && ( xCharsForTx != serINVALID_QUEUE ) )
	{
        USART_DeInit(USART2);
    
    	RCC_AHB1PeriphClockCmd(USART2_GPIO_CLK,ENABLE); //使能GPIOA时钟
    	RCC_APB1PeriphClockCmd(USART2_CLK,ENABLE);//使能Usart2时钟
     
    	//串口1对应引脚复用映射
    	GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_TX_SOURCE,USART2_TX_AF); //GPIOA2复用为USART2
    	GPIO_PinAFConfig(USART2_GPIO_PORT,USART2_RX_SOURCE,USART2_RX_AF); //GPIOA3复用为USART2
    	
    	//Usart2端口配置
    	GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN | USART2_RX_PIN; //GPIOA9与GPIOA10
    	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    	GPIO_Init(USART2_GPIO_PORT,&GPIO_InitStructure); //初始化PA2，PA3

       //Usart2 初始化设置
    	USART_InitStructure.USART_BaudRate = ulWantedBaud;//波特率设置
    	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    	USART_Init(USART2, &USART_InitStructure); //初始化串口2    

    	//Usart2 NVIC 配置
    	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
    	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=10;//抢占优先级3
    	NVIC_InitStructure.NVIC_IRQChannelSubPriority =5;		//子优先级3
    	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    	
    	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断    
    	
    	USART_Cmd(USART2, ENABLE);  //使能串口1   	
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength )
{
signed char *pxNext;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed char * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime )
{
signed portBASE_TYPE xReturn;

	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
	{
		xReturn = pdPASS;
		USART_ITConfig( USART2, USART_IT_TXE, ENABLE );
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

void vUARTInterruptHandler( void )
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
char cChar;

	if( USART_GetITStatus( USART2, USART_IT_TXE ) == SET )
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART2, cChar );
		}
		else
		{
			USART_ITConfig( USART2, USART_IT_TXE, DISABLE );		
		}		
	}
	
	if( USART_GetITStatus( USART2, USART_IT_RXNE ) == SET )
	{
		cChar = USART_ReceiveData( USART2 );
		xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
	}	
	
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}





	
