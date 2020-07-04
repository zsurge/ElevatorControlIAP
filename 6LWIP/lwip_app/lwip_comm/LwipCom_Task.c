/***************************************************************************************************
*FileName：LwipCom_Task
*Description：LWIP通用任务
*Author：xsx
*Data：2016年3月9日16:28:12
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LwipCom_Task.h"
#include "lan8720.h"
#include "lwip_comm.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define LwipComTask_PRIORITY			2			//看门狗任务优先级
const char * LwipComTaskName = "vLwipComTask";		//看门狗任务名

/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vLwipComTask( void *pvParameters );	//看门狗任务

void StartvLwipComTask(void * pram)
{
	
	xTaskCreate( vLwipComTask, LwipComTaskName, configMINIMAL_STACK_SIZE, pram, LwipComTask_PRIORITY, NULL );
}

void ChangeNetifLinkStatues(void *pvParameters)
{
	LinkState_TypeDef tempdata = 0;
	struct netif * tempnetif = (struct netif *)pvParameters;
	
	tempdata = GetPHYLinkState();
	
	if(tempdata != GetGB_LinkState())
	{
		if(tempdata == Link_Down)
			netif_set_link_down(tempnetif);
		else
			netif_set_link_up(tempnetif);
		
		SetGB_LinkState(tempdata);
	}
}

static void vLwipComTask( void *pvParameters )
{
//    printf("start vLwipComTask\r\n");
	while(1)
	{
		ChangeNetifLinkStatues(pvParameters);
		/* wait 500 ms */
		vTaskDelay(500 / portTICK_RATE_MS);
	}   
}


