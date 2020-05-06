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


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* Ensure stdint is only used by the compiler, and not the assembler. */

//��Բ�ͬ�ı��������ò�ͬ��stdint.h�ļ�
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
    extern volatile uint32_t ulHighFrequencyTimerTicks;
#endif

//����
//#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
//#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)
	
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	

/***************************************************************************************************************/
/*                                        FreeRTOS������������ѡ��                                              */
/***************************************************************************************************************/
#define configUSE_PREEMPTION			1   		//1ʹ����ռʽ�ںˣ�0ʹ��Э��
#define configCPU_CLOCK_HZ				( SystemCoreClock ) 		//CPUƵ��
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 ) 		//ʱ�ӽ���Ƶ�ʣ���������Ϊ1000�����ھ���1ms
#define configMAX_PRIORITIES			( 10 )		//��ʹ�õ�������ȼ�
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 128 )		//��������ʹ�õĶ�ջ��С
#define configMAX_TASK_NAME_LEN			( 10 )	//���������ַ�������

#define configUSE_16_BIT_TICKS			0		//ϵͳ���ļ����������������ͣ�1��ʾΪ16λ�޷������Σ�0��ʾΪ32λ�޷�������
#define configIDLE_SHOULD_YIELD			1		//Ϊ1ʱ�����������CPUʹ��Ȩ������ͬ���ȼ����û�����
#define configUSE_MUTEXES				1				 //Ϊ1ʱʹ�û����ź���
#define configQUEUE_REGISTRY_SIZE		8			//��Ϊ0ʱ��ʾ���ö��м�¼�������ֵ�ǿ��Լ�¼�Ķ��к��ź��������Ŀ��
#define configCHECK_FOR_STACK_OVERFLOW	0		//����0ʱ���ö�ջ�����⹦�ܣ����ʹ�ô˹����û������ṩһ��ջ������Ӻ��������ʹ�õĻ���ֵ����Ϊ1����2����Ϊ������ջ�����ⷽ����
#define configUSE_RECURSIVE_MUTEXES		1			//Ϊ1ʱʹ�õݹ黥���ź���

#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1			//Ϊ1ʱʹ�ü����ź���


/***************************************************************************************************************/
/*                                FreeRTOS���ڴ������й�����ѡ��                                                */
/***************************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION        1                       //֧�ֶ�̬�ڴ����� V9.0
#define configTOTAL_HEAP_SIZE					((size_t)(42*1024))     //ϵͳ�����ܵĶѴ�С

/***************************************************************************************************************/
/*                                FreeRTOS�빳�Ӻ����йص�����ѡ��                                              */
/***************************************************************************************************************/
#define configUSE_IDLE_HOOK						0                       //1��ʹ�ÿ��й��ӣ�0����ʹ��
#define configUSE_TICK_HOOK						0                       //1��ʹ��ʱ��Ƭ���ӣ�0����ʹ��
#define configUSE_MALLOC_FAILED_HOOK	0								//1ʹ���ڴ�����ʧ�ܹ��Ӻ���

/***************************************************************************************************************/
/*                                FreeRTOS������ʱ�������״̬�ռ��йص�����ѡ��                                 */
/***************************************************************************************************************/
#define configGENERATE_RUN_TIME_STATS	        1                       //Ϊ1ʱ��������ʱ��ͳ�ƹ���

//���ú�configUSE_TRACE_FACILITYΪ1�����vTaskList()��vTaskGetRunTimeStats()��prvWriteNameToBuffer()����
#define configUSE_TRACE_FACILITY				1                       //Ϊ1���ÿ��ӻ����ٵ���


#define configUSE_STATS_FORMATTING_FUNCTIONS    1  //add0527

//���ú�configGENERATE_RUN_TIME_STATSΪ1ʹ������ʱ��ͳ�ƹ��ܡ�һ������Ϊ1����������������뱻���壺
//portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
//portGET_RUN_TIME_COUNTER_VALUE()
//add 0527
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()     (ulHighFrequencyTimerTicks = 0ul)
#define portGET_RUN_TIME_COUNTER_VALUE()             ulHighFrequencyTimerTicks


																																				

/***************************************************************************************************************/
/*                                FreeRTOS��Э���йص�����ѡ��                                                  */
/***************************************************************************************************************/
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0						//Ϊ1ʱ����Э�̣�����Э���Ժ��������ļ�croutine.c
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )	//Э�̵���Ч���ȼ���?

/***************************************************************************************************************/
/*                                FreeRTOS�������ʱ���йص�����ѡ��                                            */
/***************************************************************************************************************/
/* Software timer definitions. */
#define configUSE_TIMERS				1					//Ϊ1ʱ���������ʱ��
#define configTIMER_TASK_PRIORITY		( 2 )		//�����ʱ�����ȼ�
#define configTIMER_QUEUE_LENGTH		10			//�����ʱ�����г���
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )		//�����ʱ�������ջ��С

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
/***************************************************************************************************************/
/*                                FreeRTOS��ѡ��������ѡ��                                                      */
/***************************************************************************************************************/
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1


//add 0525
#define configUSE_STATS_FORMATTING_FUNCTIONS 1


//FreeRTOS ����֪ͨ��Ĭ��ֵΪ1
//#define configUSE_TASK_NOTIFICATIONS            1





/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	

	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler
          


#endif /* FREERTOS_CONFIG_H */

