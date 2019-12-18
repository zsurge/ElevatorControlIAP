#include "client.h"

#include "lwip/opt.h"

#include "lwip/lwip_sys.h"
#include "lwip/api.h"

#include <lwip/sockets.h>
#include "tool.h"

#define MAX_RECV_LEN  1024

//uint8_t get_buf[]= "GET /forum.php HTTP/1.1\r\n \Host: www.firebbs.cn \r\n\r\n\r\n\r\n";
static void GetHTTPGETString(char* destbuff);


//http://bsgoalsmartcloud.oss-cn-shenzhen.aliyuncs.com/AccessControl/20191216/20191216141310_qfiv.0.1.bin


static void GetHTTPGETString(char* destbuff)
{
	strcpy(destbuff, "GET ");
	strcat(destbuff, "http://bsgoalsmartcloud.oss-cn-shenzhen.aliyuncs.com/AccessControl/20191216/20191216141310_qfiv.0.1.bin");
	strcat(destbuff, " HTTP/1.1");
	strcat(destbuff, "\r\n"); 
	
	//Accept: */*
	strcat(destbuff, "Accept: */*");
	strcat(destbuff, "\r\n");

	strcat(destbuff, "Content-Type: text/html\n");
	
	strcat(destbuff, "Host: ");
	strcat(destbuff, "bsgoalsmartcloud.oss-cn-shenzhen.aliyuncs.com");
	strcat(destbuff, "\r\n");

	
	//Proxy-Connection: Keep-Alive
	strcat(destbuff, "Connection: close");
	strcat(destbuff, "\r\n");	
	strcat(destbuff, "\r\n");
} 


static void client ( void* thread_param )
{
    uint32_t cnt = 0;
    uint32_t sum = 0;
	int sock = -1;
    int recelen = 0;
    char* host_ip;
    char debug_cnt[16] = {0};
    char sendbuf[512] = {0};
    uint8_t recvbuff[MAX_RECV_LEN] = {0};
	struct sockaddr_in client_addr;
    
    int writc_ret = 0;
    int connect_ret = 0;

START:

#ifdef LWIP_DNS
	struct ip_addr dns_ip;
	netconn_gethostbyname ( HOST_NAME, &dns_ip );
	host_ip = ip_ntoa ( &dns_ip );
	printf ( "host name : %s , host_ip : %s\n",HOST_NAME,host_ip );
#else
	host_ip = HOST_NAME;
#endif

    //http getÇëÇó°ü
    GetHTTPGETString(sendbuf);

    sock = socket ( AF_INET, SOCK_STREAM, 0 );
    if ( sock < 0 )
    {
        printf ( "Socket error\n" );
        vTaskDelay ( 10 );
        goto START;
    }
    
    printf ( "Socket success\n" );
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons ( PORT );
    client_addr.sin_addr.s_addr = inet_addr ( host_ip );
    memset ( & ( client_addr.sin_zero ), 0, sizeof ( client_addr.sin_zero ) );
    
    if ( connect ( sock,
                   ( struct sockaddr* ) &client_addr,
                   sizeof ( struct sockaddr ) ) == -1 )
    {
        printf ( "Connect failed!\n" );
        closesocket ( sock );
        vTaskDelay ( 10 );
         goto START;
    }                   
    
    printf ( "Connect to server successful!\n" );
    memset ( recvbuff,0,sizeof(recvbuff) );
    printf ( "\r\n***************************************************\r\n" );   
            
    writc_ret = write ( sock,sendbuf,strlen(sendbuf));
    DBG( "write ret = %d\r\n",writc_ret );   

RECV:
		while ( 1 )
		{
            memset ( recvbuff,0,sizeof(recvbuff) );
			recelen = recv ( sock, ( uint8_t* ) recvbuff, MAX_RECV_LEN, 0 );
            sprintf(debug_cnt,"the %d ",cnt++);
            DBG("recv len = %d,buff = %s\r\n",recelen,recvbuff);

            sum += recelen;
            
			if ( recelen <= 0 )
			{    
                 DBG("sum = %d\r\n",sum);
                 
                closesocket ( sock );
                vTaskDelay ( 10000 );
				goto RECV;
			}
            dbh(debug_cnt, recvbuff, recelen);
            
            printf ( "\r\n**************************************************\r\n" );
            vTaskDelay (3000);
		}
		
		memset ( recvbuff,0,sizeof(recvbuff) );

        DBG("------------------end-----------------------\r\n");

}

void client_init ( void )
{
	sys_thread_new ( "client", client, NULL, 2048, 4 );
}

