/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : bsp_time.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��27��
  ����޸�   :
  ��������   : ��ʱ����ʼ��
  ˵    �� : Ϊ�˻�ȡFreeRTOS��������Ϣ����Ҫ����һ����ʱ���������ʱ����ʱ���׼����Ҫ����
                ϵͳʱ�ӽ��ġ������õ���������Ϣ��׼ȷ��
       -------------------------------
                ���ļ�ʹ�õ���32λ����������500usһ�εļ���ֵ�����֧�ּ���ʱ�䣺
                2^32 * 500us / 3600s = 596.5���ӡ�ʹ���в��Ե��������м���������ռ���ʳ�����596.5���ӽ���׼ȷ��

  �޸���ʷ   :
  1.��    ��   : 2019��5��27��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#include "bsp_time.h"
#include "bsp_wiegand.h"

volatile uint32_t ulHighFrequencyTimerTicks = 0UL;

volatile uint32_t g500usTimerUART = 0;
volatile uint32_t g500usTimerRS485 = 0;


void bsp_TimeSysTickHandler (void)
{
    if (g500usTimerUART  > 0) g500usTimerUART--;    
    if (g500usTimerRS485  > 0) g500usTimerRS485--; 
}

void bsp_TIM6_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///ʹ��TIM6ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Period = ((SystemCoreClock/2)/100)/timerINTERRUPT_FREQUENCY -1; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=99;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//��ʼ��TIM6

    TIM_ARRPreloadConfig(TIM6, ENABLE);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //����ʱ��6�����ж�
	
	TIM_Cmd(TIM6,ENABLE); //ʹ�ܶ�ʱ��6
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; //��ʱ��6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=timerHIGHEST_PRIORITY; //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);    
}


/*
*********************************************************************************************************
*	�� �� ��: TIM6_DAC_IRQHandler
*	����˵��: TIM6�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
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





