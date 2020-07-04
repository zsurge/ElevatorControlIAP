/***************************************************************************************************
*FileName��LwipDHCP_Task
*Description��LWIP��̬��ȡIP����
*Author��xsx
*Data��2016��3��9��14:38:36
***************************************************************************************************/

/***************************************************************************************************/
/******************************************ͷ�ļ�***************************************************/
/***************************************************************************************************/

#include	"LwipDHCP_Task.h"
#include "lwip_comm.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include "pub_options.h"


/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

#define LwipDHCPTask_PRIORITY			2			//���Ź��������ȼ�
const char * LwipDHCPTaskName = "vLwipDhcpTask";		//���Ź�������

static DHCP_State_TypeDef GB_DHCPState = DHCP_START;
/***************************************************************************************************/
/**************************************�ֲ���������*************************************************/
/***************************************************************************************************/

static void vLwipDHCPTask( void *pvParameters );	//���Ź�����

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************����********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName��StartvSysLedTask
*Description������ϵͳָʾ������
*Input��None
*Output��None
*Author��xsx
*Data��2015��8��26��16:58:13
***************************************************************************************************/
void StartvLwipDHCPTask(void * pram)
{
	SetGB_DHCPState(DHCP_START);
	
	xTaskCreate( vLwipDHCPTask, LwipDHCPTaskName, configMINIMAL_STACK_SIZE, pram, LwipDHCPTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName��GetGB_DHCPState
*Description����ȡ���µ�DHCP״̬
*Input��None
*Output��DHCP״̬
*Author��xsx
*Data��2016��3��9��14:49:52
***************************************************************************************************/
DHCP_State_TypeDef GetGB_DHCPState(void)
{
	return GB_DHCPState;
}

/***************************************************************************************************
*FunctionName��SetGB_DHCPState
*Description������DHCP״̬
*Input��NewState -- ���µ�DHCP״̬
*Output��None
*Author��xsx
*Data��2016��3��9��14:50:32
***************************************************************************************************/
void SetGB_DHCPState(DHCP_State_TypeDef NewState)
{
	GB_DHCPState = NewState;
}

/***************************************************************************************************
*FunctionName��vSysLedTask
*Description��ϵͳָʾ����˸���������������
*Input��None
*Output��None
*Author��xsx
*Data��2015��8��26��16:58:46
***************************************************************************************************/
static void vLwipDHCPTask( void *pvParameters )
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
    
	unsigned int IPaddress;
	struct netif * tempnetif = (struct netif *)pvParameters;

    uint32_t ip=0,nk=0,gww=0;
  
//    uint8_t lwipdevip[4] = {0};       //����IP��ַ
//	uint8_t lwipdevnetmask[4] = {0}; 	//��������
//	uint8_t lwipdevgateway[4] = {0};


//    printf("start vLwipDHCPTask\r\n");
	while(1)
	{
		switch (GetGB_DHCPState())
		{
			case DHCP_START:
//                printf("dhcp statr\r\n");;
				{
					IPaddress = 0;
					
					dhcp_start(tempnetif);
					
					SetGB_DHCPState(DHCP_WAIT_ADDRESS);
				}
				break;

			case DHCP_WAIT_ADDRESS:
//                printf("DHCP_WAIT_ADDRESS\r\n");
				{
					/* Read the new IP address */
					IPaddress = tempnetif->ip_addr.addr;
					printf("���ڻ�ȡIP.....\r\n");
					if (IPaddress!=0) 
					{
						SetGB_DHCPState(DHCP_ADDRESS_ASSIGNED);		

//                        printf("DHCP_ADDRESS_ASSIGNED\r\n");

                        ip=tempnetif->ip_addr.addr;		//��ȡ��IP��ַ
                		nk=tempnetif->netmask.addr;//��ȡ��������
                		gww=tempnetif->gw.addr;			//��ȡĬ������
                		
                        //������ͨ��DHCP��ȡ����IP��ַ
                        lwipdev.ip[3]= ( uint8_t ) ( ip>>24 );
                        lwipdev.ip[2]= ( uint8_t ) ( ip>>16 );
                        lwipdev.ip[1]= ( uint8_t ) ( ip>>8 );
                        lwipdev.ip[0]= ( uint8_t ) ( ip );
                        printf ( "ͨ��DHCP��ȡ��IP��ַ..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                        //����ͨ��DHCP��ȡ�������������ַ
                        lwipdev.netmask[3]= ( uint8_t ) ( nk>>24 );
                        lwipdev.netmask[2]= ( uint8_t ) ( nk>>16 );
                        lwipdev.netmask[1]= ( uint8_t ) ( nk>>8 );
                        lwipdev.netmask[0]= ( uint8_t ) ( nk );
                        printf ( "ͨ��DHCP��ȡ����������............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                        //������ͨ��DHCP��ȡ����Ĭ������
                        lwipdev.gateway[3]= ( uint8_t ) ( gww>>24 );
                        lwipdev.gateway[2]= ( uint8_t ) ( gww>>16 );
                        lwipdev.gateway[1]= ( uint8_t ) ( gww>>8 );
                        lwipdev.gateway[0]= ( uint8_t ) ( gww );
                        printf ( "ͨ��DHCP��ȡ����Ĭ������..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
                        
                        printf ( "������MAC��ַΪ:................%02x.%02x.%02x.%02x.%02x.%02x\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5] );
						/* Stop DHCP */
						dhcp_stop(tempnetif);
						xTaskNotifyGive(xHandleTaskMqtt);
					}
					else
					{
						/* DHCP timeout */
						if (tempnetif->dhcp->tries > LWIP_MAX_DHCP_TRIES)
						{
							SetGB_DHCPState(DHCP_TIMEOUT);
							printf("IP��ȡʧ��.....\r\n");
							/* Stop DHCP */
							dhcp_stop(tempnetif);
                            
                            /* Static address used */
                            IP4_ADDR ( &ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                            IP4_ADDR ( &netmask,lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                            IP4_ADDR ( &gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
                            
                            printf ( "������MAC��ַΪ:................%02x.%02x.%02x.%02x.%02x.%02x\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5] );
                            printf ( "��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                            printf ( "��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                            printf ( "Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
                            xTaskNotifyGive(xHandleTaskMqtt);
						}
					}
				}
				break;

			default: break;
		}
    
		/* wait 250 ms */
		vTaskDelay(250 / portTICK_RATE_MS);
	}   
}


