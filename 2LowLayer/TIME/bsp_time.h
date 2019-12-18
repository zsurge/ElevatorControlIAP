#ifndef __BSP_TIME_H
#define __BSP_TIME_H

#include "stm32f4xx_conf.h"

/*
       SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
    -- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
    -- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
    -- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us
    -- SystemCoreClock / 20000  表示定时频率为 20000Hz， 也就是定时周期为  50us
*/   


/* 定时器频率，500us一次中断 */
#define  timerINTERRUPT_FREQUENCY	2000

/* 中断优先级 */
#define  timerHIGHEST_PRIORITY		2

/* 被系统调用 */


extern volatile uint32_t g1msTimerUART1;
extern volatile uint32_t g1msTimerUART2;
extern volatile uint32_t g1msTimerUART3;
extern volatile uint32_t g1msTimerUART4;
extern volatile uint32_t g1msTimerUART5;
extern volatile uint32_t g1msTimerUART;





void bsp_TimeSysTickHandler (void);
void bsp_TIM6_Init(void);

#endif



