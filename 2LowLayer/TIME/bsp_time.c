/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_time.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月27日
  最近修改   :
  功能描述   : 定时器初始化
  说    明 : 为了获取FreeRTOS的任务信息，需要创建一个定时器，这个定时器的时间基准精度要高于
                系统时钟节拍。这样得到的任务信息才准确。
       -------------------------------
                本文件使用的是32位变量来保存500us一次的计数值，最大支持计数时间：
                2^32 * 500us / 3600s = 596.5分钟。使用中测试的任务运行计数和任务占用率超过了596.5分钟将不准确。

  修改历史   :
  1.日    期   : 2019年5月27日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#include "bsp_time.h"
#include "bsp_wiegand.h"

volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

volatile uint32_t g1msTimerUART = 0;
volatile uint32_t g1msTimerUART1 = 0;
volatile uint32_t g1msTimerUART2 = 0;
volatile uint32_t g1msTimerUART3 = 0;
volatile uint32_t g1msTimerUART4 = 0;
volatile uint32_t g1msTimerUART5 = 0;






void bsp_TimeSysTickHandler (void)
{
    if (g1msTimerUART  > 0) g1msTimerUART--;    
	if (g1msTimerUART1 > 0) g1msTimerUART1--;
    if (g1msTimerUART2 > 0) g1msTimerUART2--;
    if (g1msTimerUART3 > 0) g1msTimerUART3--;
    if (g1msTimerUART4 > 0) g1msTimerUART4--;
    if (g1msTimerUART5 > 0) g1msTimerUART5--;
}

void bsp_TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///使能TIM6时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = ((SystemCoreClock/2)/100)/timerINTERRUPT_FREQUENCY -1; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=99;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//初始化TIM6

    TIM_ARRPreloadConfig(TIM6, ENABLE);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //允许定时器6更新中断
	
	TIM_Cmd(TIM6,ENABLE); //使能定时器6
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; //定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=timerHIGHEST_PRIORITY; //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);    
}


/*
*********************************************************************************************************
*	函 数 名: TIM6_DAC_IRQHandler
*	功能说明: TIM6中断服务程序。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM6_DAC_IRQHandler( void )
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		ulHighFrequencyTimerTicks++;
        bsp_TimeSysTickHandler();

        if(WG_Rx_Str.END_TIME_C_EN)
        {
            if(WG_Rx_Str.End_TIME)
                WG_Rx_Str.End_TIME--; 
        }
        
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}





