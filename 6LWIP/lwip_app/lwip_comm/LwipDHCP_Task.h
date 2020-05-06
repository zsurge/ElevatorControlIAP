#ifndef _LWIPDHCP_T_H__
#define _LWIPDHCP_T_H__

#include "tcpip.h"
#include "lwip/dhcp.h"

#define LWIP_MAX_DHCP_TRIES		4   //DHCP服务器最大重试次数


typedef enum 
{ 
	DHCP_START=0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_TIMEOUT,
	DHCP_LINK_DOWN
}DHCP_State_TypeDef;


void StartvLwipDHCPTask(void * pram);
DHCP_State_TypeDef GetGB_DHCPState(void);
void SetGB_DHCPState(DHCP_State_TypeDef NewState);


#endif




