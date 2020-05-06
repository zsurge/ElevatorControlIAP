#ifndef __LWIPCONFIG_H
#define __LWIPCONFIG_H

//DHCP最大重试次数
#define	MAX_DHCP_TRIES	5

#define MAC_ADDR0   02
#define MAC_ADDR1   00
#define MAC_ADDR2   00
#define MAC_ADDR3   00
#define MAC_ADDR4   00
#define MAC_ADDR5   00
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   2
#define IP_ADDR3   110
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   2
#define GW_ADDR3   1	


#endif
