#ifndef __BSP_TIME_H
#define __BSP_TIME_H

#include "stm32f4xx_conf.h"

/*
       SysTick_Config() �������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�.
    -- SystemCoreClock / 1000  ��ʾ��ʱƵ��Ϊ 1000Hz�� Ҳ���Ƕ�ʱ����Ϊ  1ms
    -- SystemCoreClock / 500   ��ʾ��ʱƵ��Ϊ 500Hz��  Ҳ���Ƕ�ʱ����Ϊ  2ms
    -- SystemCoreClock / 2000  ��ʾ��ʱƵ��Ϊ 2000Hz�� Ҳ���Ƕ�ʱ����Ϊ  500us
    -- SystemCoreClock / 20000  ��ʾ��ʱƵ��Ϊ 20000Hz�� Ҳ���Ƕ�ʱ����Ϊ  50us
*/   


/* ��ʱ��Ƶ�ʣ�500usһ���ж� */
#define  timerINTERRUPT_FREQUENCY	2000

/* �ж����ȼ� */
#define  timerHIGHEST_PRIORITY		2

/* ��ϵͳ���� */

extern volatile uint32_t g500usTimerUART;
extern volatile uint32_t g500usTimerRS485;





void bsp_TimeSysTickHandler (void);
void bsp_TIM6_Init(void);

#endif



