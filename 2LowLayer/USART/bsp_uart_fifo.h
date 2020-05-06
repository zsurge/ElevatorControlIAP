/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*	文件名称 : bsp_uart_fifo.h
*	说    明 : 头文件
*
*	Copyright (C), 2015-2020, 
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stdio.h"	
#include "string.h"
#include "stm32f4xx_conf.h"


#define  USE_FreeRTOS      1

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	/* 开关全局中断的宏 */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */
#endif



/*
	如果需要更改串口对应的管脚，请自行修改 bsp_uart_fifo.c文件中的 static void InitHardUart(void)函数
*/

/* 定义使能的串口, 0 表示不使能（不增加代码大小）， 1表示使能 */
#define	UART1_FIFO_EN	0
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0


/* 定义使能的485接口, 0 表示不使能（不增加代码大小）， 1表示使能 */
#define UART1_RS485_EN	0
#define	UART2_RS485_EN	0
#define	UART3_RS485_EN	0
#define	UART4_RS485_EN	0
#define	UART5_RS485_EN	0
#define	UART6_RS485_EN	0




/* RS485芯片发送使能GPIO, PE1 */
#define RS485_U2_RX_EN()	    GPIOE->BSRRH = GPIO_Pin_1
#define RS485_U2_TX_EN()	    GPIOE->BSRRL = GPIO_Pin_1

/* RS485芯片发送使能GPIO, PE5 */
#define RS485_U1_RX_EN()	    GPIOE->BSRRH = GPIO_Pin_5
#define RS485_U1_TX_EN()	    GPIOE->BSRRL = GPIO_Pin_5







/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1  PB6, PB7*/
	COM2 = 1,	/* USART2, PD5,PD6 或 PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
	COM6 = 5	/* USART6, PC6, PC7 */
}COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			38400
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			38400
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			38400
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			38400
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			38400
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

#if UART6_FIFO_EN == 1
	#define UART6_BAUD			38400
	#define UART6_TX_BUF_SIZE	1*1024
	#define UART6_RX_BUF_SIZE	1*1024
#endif

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;	/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;		/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;	/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;	/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;		/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;	/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
//uint16_t comGetBuff(COM_PORT_E _ucPort,uint8_t *Buff, uint16_t RecvSize);
uint8_t comRecvBuff(COM_PORT_E _ucPort,uint8_t *buf, uint16_t len);
uint16_t comGetBuff(COM_PORT_E _ucPort,uint8_t *Buff, uint16_t RecvSize,uint16_t timeout_MilliSeconds);

void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);
void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate);



void RS485_InitTXE(void);
void RS485_SendBuf(COM_PORT_E _ucPort,uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(COM_PORT_E _ucPort,char *_pBuf);
void RS485_SetBaud(COM_PORT_E _ucPort,uint32_t _baud);

uint8_t RS485_Recv(COM_PORT_E _ucPort,uint8_t *buf, uint8_t len);
uint8_t RS485_RecvAtTime(COM_PORT_E _ucPort,uint8_t *buf, uint8_t len,uint32_t timeout);




#endif



