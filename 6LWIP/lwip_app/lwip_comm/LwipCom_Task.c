/***************************************************************************************************
*FileName��LwipCom_Task
*Description��LWIPͨ������
*Author��xsx
*Data��2016��3��9��16:28:12
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"LwipCom_Task.h"
#include "lan8720.h"
#include "lwip_comm.h"

#include 	"FreeRTOS.h"
#include 	"task.h"

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

#define LwipComTask_PRIORITY			2			//���Ź��������ȼ�
const char * LwipComTaskName = "vLwipComTask";		//���Ź�������

/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

static void vLwipComTask( void *pvParameters );	//���Ź�����

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


