/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	˵    �� : ͷ�ļ�
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
	/* ����ȫ���жϵĺ� */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
#endif



/*
	�����Ҫ���Ĵ��ڶ�Ӧ�Ĺܽţ��������޸� bsp_uart_fifo.c�ļ��е� static void InitHardUart(void)����
*/

/* ����ʹ�ܵĴ���, 0 ��ʾ��ʹ�ܣ������Ӵ����С���� 1��ʾʹ�� */
#define	UART1_FIFO_EN	0
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0


/* ����ʹ�ܵ�485�ӿ�, 0 ��ʾ��ʹ�ܣ������Ӵ����С���� 1��ʾʹ�� */
#define UART1_RS485_EN	0
#define	UART2_RS485_EN	0
#define	UART3_RS485_EN	0
#define	UART4_RS485_EN	0
#define	UART5_RS485_EN	0
#define	UART6_RS485_EN	0




/* RS485оƬ����ʹ��GPIO, PE1 */
#define RS485_U2_RX_EN()	    GPIOE->BSRRH = GPIO_Pin_1
#define RS485_U2_TX_EN()	    GPIOE->BSRRL = GPIO_Pin_1

/* RS485оƬ����ʹ��GPIO, PE5 */
#define RS485_U1_RX_EN()	    GPIOE->BSRRH = GPIO_Pin_5
#define RS485_U1_TX_EN()	    GPIOE->BSRRL = GPIO_Pin_5







/* ����˿ں� */
typedef enum
{
	COM1 = 0,	/* USART1  PB6, PB7*/
	COM2 = 1,	/* USART2, PD5,PD6 �� PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
	COM6 = 5	/* USART6, PC6, PC7 */
}COM_PORT_E;

/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
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

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
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



