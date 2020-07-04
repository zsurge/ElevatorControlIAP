#include "lan8720.h" 
#include "lwip_comm.h"

#include	"LwipDHCP_Task.h"
#include	"LwipCom_Task.h"

#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h"
#include "lwip/timers.h"
#include "malloc.h"
#include "delay.h"
#include <stdio.h>
#include <time.h>
#include "tool.h"

__lwip_dev lwipdev;						//lwip���ƽṹ��

struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

static LinkState_TypeDef GB_LinkState = Link_Down; 
void ETH_link_callback(struct netif *netif);


extern u32 memp_get_memorysize ( void );	//��memp.c���涨��
extern u8_t* memp_memory;				//��memp.c���涨��.
extern u8_t* ram_heap;					//��mem.c���涨��.

//������̫���жϵ���
void lwip_pkt_handle ( void )
{
	ethernetif_input ( &lwip_netif );
}

//lwip��mem��memp���ڴ�����
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 lwip_comm_mem_malloc ( void )
{
	u32 mempsize;
	u32 ramheapsize;
	mempsize=memp_get_memorysize();			//�õ�memp_memory�����С
	memp_memory=mymalloc ( SRAMIN,mempsize );	//Ϊmemp_memory�����ڴ�
	ramheapsize=LWIP_MEM_ALIGN_SIZE ( MEM_SIZE )+2*LWIP_MEM_ALIGN_SIZE ( 4*3 )+MEM_ALIGNMENT; //�õ�ram heap��С
	ram_heap=mymalloc ( SRAMIN,ramheapsize );	//Ϊram_heap�����ڴ�
	if ( !memp_memory||!ram_heap ) //������ʧ�ܵ�
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;
}
//lwip��mem��memp�ڴ��ͷ�
void lwip_comm_mem_free ( void )
{
	myfree ( SRAMIN,memp_memory );
	myfree ( SRAMIN,ram_heap );
}
//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set ( __lwip_dev* lwipx )
{
//    //02:00:00:52:48:50
//	uint32_t sn0 = *(__I uint32_t *)(0x1FFF7A10+0x08); //��̬MAC

	//Ĭ��Զ��IPΪ:192.168.1.100
	lwipx->remoteip[0]=192;
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=110;
	lwipx->remoteip[3]=78;

	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
//	lwipx->mac[0]=2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
//	lwipx->mac[1]=0;
//	lwipx->mac[2]=0;
//	lwipx->mac[3]=(sn0>>16)&0XFF;//�����ֽ���STM32��ΨһID
//	lwipx->mac[4]=(sn0>>8)&0XFFF;
//	lwipx->mac[5]=sn0&0XFF;

	calcMac ( lwipx->mac );

	//Ĭ�ϱ���IPΪ:192.168.1.30
	lwipx->ip[0]=192;
	lwipx->ip[1]=168;
	lwipx->ip[2]=110;
	lwipx->ip[3]=98;
	//Ĭ����������:255.255.255.0
	lwipx->netmask[0]=255;
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//Ĭ������:192.168.1.1
	lwipx->gateway[0]=192;
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=110;
	lwipx->gateway[3]=1;
	lwipx->dhcpstatus=0;//û��DHCP
}

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,LAN8720��ʼ��ʧ��
//      3,�������ʧ��.
u8 lwip_comm_init ( void )
{

	struct netif* Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip_addr ipaddr;  			//ip��ַ
	struct ip_addr netmask; 			//��������
	struct ip_addr gw;      			//Ĭ������

	if ( ETH_Mem_Malloc() )
	{
		return 1;    //�ڴ�����ʧ��
	}
//	printf ( "ETH_Mem_Malloc\r\n" );

	if ( lwip_comm_mem_malloc() )
	{
		return 2;    //�ڴ�����ʧ��
	}
//	printf ( "lwip_comm_mem_malloc\r\n" );

	lwip_comm_default_ip_set ( &lwipdev );	//����Ĭ��IP����Ϣ
//	printf ( "lwip_comm_default_ip_set\r\n" );

    LAN8720_Init();
    
	tcpip_init ( NULL,NULL );				//��ʼ��tcp ip�ں�,�ú�������ᴴ��tcpip_thread�ں�����
//	printf ( "tcpip_init\r\n" );

#if LWIP_DHCP		//ʹ�ö�̬IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//ʹ�þ�̬IP
	IP4_ADDR ( &ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
	IP4_ADDR ( &netmask,lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
	IP4_ADDR ( &gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
	printf ( "������MAC��ַΪ:................%02x.%02x.%02x.%02x.%02x.%02x\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5] );
	printf ( "��̬IP��ַ........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3] );
	printf ( "��������..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3] );
	printf ( "Ĭ������..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3] );
#endif

	Netif_Init_Flag=netif_add ( &lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input ); //�������б������һ������
	if ( Netif_Init_Flag==NULL )
	{
		return 4;    //�������ʧ��
	}
	else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default ( &lwip_netif ); //����netifΪĬ������	
	}

	if(Link_Down == GetGB_LinkState())
	{
		netif_set_up(&lwip_netif); //��netif����
		
		#ifdef LWIP_DHCP
			SetGB_DHCPState(DHCP_START);
		#endif
	}
	else
	{
		/*  When the netif link is down this function must be called.*/
		netif_set_down(&lwip_netif);
		
		#ifdef LWIP_DHCP
			SetGB_DHCPState(DHCP_LINK_DOWN);
		#endif
	}    

	/* Set the link callback function, this function is called on change of link status*/
	netif_set_link_callback(&lwip_netif, ETH_link_callback);

    
	return 0;//����OK.
}



/***************************************************************************************************
*FunctionName��SetGB_LinkState
*Description��������������״̬
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��9��15:16:33
***************************************************************************************************/
void SetGB_LinkState(LinkState_TypeDef linkstate)
{
	GB_LinkState = linkstate;
}
/***************************************************************************************************
*FunctionName��GetGB_LinkState
*Description����ȡ��������״̬
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��9��15:16:48
***************************************************************************************************/
LinkState_TypeDef GetGB_LinkState(void)
{
	return GB_LinkState;
}

/***************************************************************************************************
*FunctionName��ETH_link_callback
*Description����������״̬�ı�ص�����
*Input��None
*Output��None
*Author��xsx
*Data��2016��3��9��17:02:01
***************************************************************************************************/
void ETH_link_callback(struct netif *netif)
{

	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	#ifndef LWIP_DHCP
		uint8_t iptab[4] = {0};
		uint8_t iptxt[20];
	#endif /* LWIP_DHCP */


	if(netif_is_link_up(netif))
	{
		printf("<<<<<<<<<<<<<<<<link up>>>>>>>>>>>>>>>>> \r\n");
		/* Restart MAC interface */
		ETH_Start();

		#ifdef LWIP_DHCP
			ipaddr.addr = 0;
			netmask.addr = 0;
			gw.addr = 0;

			SetGB_DHCPState(DHCP_START);
		#else
			IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		#endif /* LWIP_DHCP */

		netif_set_addr(&lwip_netif, &ipaddr , &netmask, &gw);
    
		/* When the netif is fully configured this function must be called.*/
		netif_set_up(&lwip_netif);    
	}
	else
	{
		printf("<<<<<<<<<<<<<<<<link down>>>>>>>>>>>>>>>>> \r\n");
		ETH_Stop();
		#ifdef LWIP_DHCP
			SetGB_DHCPState(DHCP_LINK_DOWN);
			dhcp_stop(netif);
		#endif /* LWIP_DHCP */

		/*  When the netif link is down this function must be called.*/
		netif_set_down(&lwip_netif);
	}
}


void StartEthernet(void)
{
	//* ��ʼ��LwIP
	lwip_comm_init();
	
#if LWIP_DHCP
    StartvLwipDHCPTask(&lwip_netif);
#endif
	
	StartvLwipComTask(&lwip_netif);
}





