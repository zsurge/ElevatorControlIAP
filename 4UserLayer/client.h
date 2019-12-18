#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#define PORT 80
#define IP_ADDR "120.77.166.124"

#ifdef LWIP_DNS
#define HOST_NAME "bsgoalsmartcloud.oss-cn-shenzhen.aliyuncs.com"
#else
#define HOST_NAME "120.77.166.124"
#endif


void client_init(void);




#endif /* TCP_CLIENT_H */

