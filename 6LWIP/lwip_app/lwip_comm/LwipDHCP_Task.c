/***************************************************************************************************
*FileName：LwipDHCP_Task
*Description：LWIP动态获取IP任务
*Author：xsx
*Data：2016年3月9日14:38:36
***************************************************************************************************/

/***************************************************************************************************/
/******************************************头文件***************************************************/
/***************************************************************************************************/

#include	"LwipDHCP_Task.h"
#include "lwip_comm.h"

#include 	"FreeRTOS.h"
#include 	"task.h"
#include "pub_options.h"


/***************************************************************************************************/
/**************************************局部变量声明*************************************************/
/***************************************************************************************************/

#define LwipDHCPTask_PRIORITY			2			//看门狗任务优先级
const char * LwipDHCPTaskName = "vLwipDhcpTask";		//看门狗任务名

static DHCP_State_TypeDef GB_DHCPState = DHCP_START;
/***************************************************************************************************/
/**************************************局部函数声明*************************************************/
/***************************************************************************************************/

static void vLwipDHCPTask( void *pvParameters );	//看门狗任务

/***************************************************************************************************/
/***************************************************************************************************/
/***************************************正文********************************************************/
/***************************************************************************************************/
/***************************************************************************************************/
/***************************************************************************************************/

/***************************************************************************************************
*FunctionName：StartvSysLedTask
*Description：建立系统指示灯任务
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:13
***************************************************************************************************/
void StartvLwipDHCPTask(void * pram)
{
	SetGB_DHCPState(DHCP_START);
	
	xTaskCreate( vLwipDHCPTask, LwipDHCPTaskName, configMINIMAL_STACK_SIZE, pram, LwipDHCPTask_PRIORITY, NULL );
}

/***************************************************************************************************
*FunctionName：GetGB_DHCPState
*Description：获取最新的DHCP状态
*Input：None
*Output：DHCP状态
*Author：xsx
*Data：2016年3月9日14:49:52
***************************************************************************************************/
DHCP_State_TypeDef GetGB_DHCPState(void)
{
	return GB_DHCPState;
}

/***************************************************************************************************
*FunctionName：SetGB_DHCPState
*Description：更新DHCP状态
*Input：NewState -- 最新的DHCP状态
*Output：None
*Author：xsx
*Data：2016年3月9日14:50:32
***************************************************************************************************/
void SetGB_DHCPState(DHCP_State_TypeDef NewState)
{
	GB_DHCPState = NewState;
}

/***************************************************************************************************
*FunctionName：vSysLedTask
*Description：系统指示灯闪烁表面程序正常运行
*Input：None
*Output：None
*Author：xsx
*Data：2015年8月26日16:58:46
***************************************************************************************************/
static void vLwipDHCPTask( void *pvParameters )
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
    
	unsigned int IPaddress;
	struct netif * tempnetif = (struct netif *)pvParameters;

    uint32_t ip=0,nk=0,gww=0;
  
//    uint8_t lwipdevip[4] = {0};       //本机IP地址
//	uint8_t lwipdevnetmask[4] = {0}; 	//子网掩码
//	uint8_t lwipdevgateway[4] = {0};


    printf("start vLwipDHCPTask\r\n");
	while(1)
	{
		switch (GetGB_DHCPState())
		{
			case DHCP_START:
                printf("dhcp statr\r\n");;
				{
					IPaddress = 0;
					
					dhcp_start(tempnetif);
					
					SetGB_DHCPState(DHCP_WAIT_ADDRESS);
				}
				break;

			case DHCP_WAIT_ADDRESS:
                printf("DHCP_WAIT_ADDRESS\r\n");
				{
					/* Read the new IP address */
					IPaddress = tempnetif->ip_addr.addr;
					printf("正在获取IP.....\r\n");
					if (IPaddress!=0) 
					{
						SetGB_DHCPState(DHCP_ADDRESS_ASSIGNED);		

                        printf("DHCP_ADDRESS_ASSIGNED\r\n");

                        ip=tempnetif->ip_addr.addr;		//读取新IP地址
                		nk=tempnetif->netmask.addr;//读取子网掩码
                		gww=tempnetif->gw.addr;			//读取默认网关
                		
                        //解析出通过DHCP获取到的IP地址
                        lwipdev.ip[3]= ( uint8_t ) ( ip>>24 );
                        lwipdev.ip[2]= ( uint8_t ) ( ip>>16 );
                        lwipdev.ip[1]= ( uint8_t ) ( ip>>8 );
                        lwipdev.ip[0]= ( uint8_t ) ( ip );
                        printf ( "通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                        //解析通过DHCP获取到的子网掩码地址
                        lwipdev.netmask[3]= ( uint8_t ) ( nk>>24 );
                        lwipdev.netmask[2]= ( uint8_t ) ( nk>>16 );
                        lwipdev.netmask[1]= ( uint8_t ) ( nk>>8 );
                        lwipdev.netmask[0]= ( uint8_t ) ( nk );
                        printf ( "通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                        //解析出通过DHCP获取到的默认网关
                        lwipdev.gateway[3]= ( uint8_t ) ( gww>>24 );
                        lwipdev.gateway[2]= ( uint8_t ) ( gww>>16 );
                        lwipdev.gateway[1]= ( uint8_t ) ( gww>>8 );
                        lwipdev.gateway[0]= ( uint8_t ) ( gww );
                        printf ( "通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
                        
                        printf ( "网卡的MAC地址为:................%02x.%02x.%02x.%02x.%02x.%02x\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5] );
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
							printf("IP获取失败.....\r\n");
							/* Stop DHCP */
							dhcp_stop(tempnetif);
                            
                            /* Static address used */
                            IP4_ADDR ( &ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                            IP4_ADDR ( &netmask,lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                            IP4_ADDR ( &gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
                            
                            printf ( "网卡的MAC地址为:................%02x.%02x.%02x.%02x.%02x.%02x\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5] );
                            printf ( "静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
                            printf ( "子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
                            printf ( "默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
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


