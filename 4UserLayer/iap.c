/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : iap.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��4��
  ����޸�   :
  ��������   : ������������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��4��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "iap.h"
#include <stdio.h>
#include "easyflash.h"
#include "sfud.h"
#include "stmflash.h"
#include "tool.h"

#include "client.h"
#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>


#define LOG_TAG    "IAP"
#include "elog.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define READ_BIN_BUFFER_SIZE 2048

#define CONNECT_OK       1
#define DIS_CONNECT     -1

#define HTTP_SEND_OK "200 OK"
#define HTTP_FILELENGTH "Content-Length:"
#define HTTP_LENGTH_ENDFLAG "\r\n"
#define HTTP_FILE_ENDFLAG "\r\n\r\n"
ReceriveType gBinQueue;

#define MAX_RECV_LEN  1024
static void GetHTTPGETString(char* destbuff,char *url);
static char HttpConnect(void);


static uint32_t update_file_cur_size =0;

static int sock = -1;


typedef  void (*pFunction)(void); 
pFunction Jump_To_Application;

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

static void GetHTTPGETString(char* destbuff,char *url)
{
	strcpy(destbuff, "GET ");
	//strcat(destbuff, "http://bsgoalsmartcloud.oss-cn-shenzhen.aliyuncs.com/AccessControl/20191216/20191216141310_qfiv.0.1.bin");
    strcat(destbuff,url);
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

uint8_t IAP_JumpToApplication(void)
//void IAP_JumpToApplication(void)
{
    uint32_t JumpAddress = 0;//��ת��ַ
    
    if ( ( (* (volatile uint32_t*) APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
//        __set_PRIMASK(1);
        __disable_irq(); 
        
		/* Jump to user application */
		JumpAddress = *(volatile uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;

		/* Initialize user application's Stack Pointer */
		__set_PSP(*(volatile uint32_t*) APPLICATION_ADDRESS);        

		Jump_To_Application();	
        
		while(1);
    }    
    else
    {
        return 1;
    }
}

//�����ļ���д���ⲿFLASH
int32_t IAP_DownLoadToFlash(void)
{
    uint8_t upsteps = HTTP_CONNECT;     //��Ϣ״̬��ʼ��
    int fileLen = 0;
    int index = 0;

    char *url;

    int recelen = 0; 
    char sendbuf[512] = {0};
    uint8_t recvbuff[MAX_RECV_LEN] = {0};

    uint8_t pageBuff[PAGE_SIZE] = {0};

    uint32_t DATAS_LENGTH = PAGE_SIZE;


    static uint32_t ContentLength = 0;
    static uint32_t send = 0;

  
    
    //���ӵ�������
    if(HttpConnect() != CONNECT_OK)
    {    
        return 0;
    }   

    while(1)
    { 
        if(upsteps > HTTP_CONNECT)
        {
            memset ( recvbuff,0,sizeof(recvbuff) );
            recelen = 0;
            recelen = recv ( sock, ( uint8_t* ) recvbuff, MAX_RECV_LEN, 0 );
            
            if(recelen > 0)
            {                
                WriteBinQueue(&gBinQueue,recvbuff,recelen);
                log_d("1.recelen = %d,count = %d\r\n",recelen,gBinQueue.Count);
            }                                  
        }      
    
        switch(upsteps)
        {

            //
            case HTTP_CONNECT:  
                            log_d("--------HTTP_CONNECT--------\r\n");
                            //http get�����
                            url = ef_get_env("url");
                            log_d("url: %s\r\n",url);
                            GetHTTPGETString(sendbuf,url);
                            write ( sock,sendbuf,strlen(sendbuf));
                            upsteps = HTTP_CONNACK;
                            break; 
                            
            case HTTP_CONNACK:
                            log_d("--------HTTP_CONNACK--------\r\n");
                            //ȷ�Ϸ����Ƿ�OK 200 OK
                            if(strstr_t(recvbuff,HTTP_SEND_OK) == NULL)
                            {
                                //http ����ʧ��,���·��ͣ��ٽ���
                            }   
                            log_d("http request success!\r\n");
                            
                            //��ȡHTTP������
                            fileLen = getFileLength(recvbuff, HTTP_FILELENGTH, HTTP_LENGTH_ENDFLAG);
                            if(fileLen <= 0)
                            {
                                //http ����ʧ��,���·��ͣ��ٽ���
                            }
                            log_d("file length = %d\r\n",fileLen);
                            
                            //ef_set_env((const char *)"FileSize",(const char *)fileLen);
                            
                            ContentLength = fileLen;
                            
                            ef_erase_bak_app( fileLen);   //������Ӧ��flash�ռ�
                            

                            index = getFileBegin(recvbuff,HTTP_FILE_ENDFLAG);
                            if(index <= 0)
                            {
                                //http ����ʧ��,���·��ͣ��ٽ���
                                log_d("get index error\r\n");
                            }

                            if(recelen > index)
                            {
//                                memcpy(pageBuff,recvbuff+index,recelen-index);
//                                dbh("first", pageBuff,recelen-index);
//                                WriteBinQueue(&gBinQueue,pageBuff,recelen-index);                                 
                                log_d("2.0 count = %d\r\n",gBinQueue.Count);
                                ReadBinQueue(&gBinQueue,pageBuff,index);
                                log_d("2.1 count = %d\r\n",gBinQueue.Count); 
                                upsteps = HTTP_SAVE_DATA;
                            }
                            break;                            
            //
            case HTTP_SAVE_DATA:

                            log_d("--------HTTP_SAVE_DATA--------\r\n"); 

                            if(gBinQueue.Count < DATAS_LENGTH)
                            {                                 
                                continue;
                            }  

                            if(send == fileLen && ContentLength == 0)
                            {                                
                                upsteps = HTTP_CLOSE;
                                break;
                            }

                            memset(pageBuff,0x00,sizeof(pageBuff));
                            ReadBinQueue(&gBinQueue,pageBuff,DATAS_LENGTH);                           
                            log_d("3.count = %d\r\n",gBinQueue.Count);
//                            dbh("->", pageBuff, DATAS_LENGTH);

                            //if(ef_write_data_to_bak(tmpBuf, 1024,&update_file_cur_size,fileLen) == EF_NO_ERR)                                        
                            if(ef_write_data_to_bak(pageBuff, DATAS_LENGTH,&update_file_cur_size,fileLen) != EF_NO_ERR) 
                            {  
                                log_e("write bin file error!\r\n");
                            }

                             log_d("update_file_cur_size = %d\r\n",update_file_cur_size);
                            
                       	    ContentLength -= DATAS_LENGTH;	
                            send += DATAS_LENGTH;
                            log_d("%dbytes has been send,Remaining %d bytes\r\n",send,ContentLength);
                            
            				if (ContentLength >= PAGE_SIZE)
                                DATAS_LENGTH = PAGE_SIZE;
            				else 
                                DATAS_LENGTH = ContentLength;      


                            break;                            
            //
            case HTTP_CLOSE:
                            log_e("send = %d,ContentLength= %d,count = %d\r\n",send,ContentLength,gBinQueue.Count);
                            log_d("--------HTTP_CLOSE--------\r\n");
                            closesocket ( sock );
                                             
//                          NVIC_SystemReset();
                            return fileLen;

            default:
                            break;

        }

    }
    closesocket(sock);
    log_d("HttpUpgrade thread exit.\r\n");

    return 0;

}





int32_t IAP_DownLoadToSTMFlash(int32_t filesize)
{
	int32_t sent=0;
	int32_t rest=filesize;
    uint8_t read_buf[READ_BIN_BUFFER_SIZE];
    uint32_t flash_addr = ef_get_bak_app_start_addr();
    uint32_t  user_app_addr,ramsource;
    size_t DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;

    
    user_app_addr = APPLICATION_ADDRESS;
    
    //����û�APP����
    STM_FLASH_Erase ( user_app_addr );    

    //���ⲿFLASH��ȡ����
    while( sent < filesize)
    {
        memset (read_buf, 0, sizeof (read_buf) );
        result = sfud_read(flash, flash_addr, DATAS_LENGTH, read_buf);     
        
        if(result != SFUD_SUCCESS)
        {
            //����
            return 3;        
        }

        ramsource = ( uint32_t ) read_buf;
        
        if ( STM_FLASH_Write (&user_app_addr, ( uint32_t* ) ramsource, ( uint16_t ) DATAS_LENGTH/4 )  != 0 ) //ֱ��дƬ��FLASH
        {
            //����
            return 2;
        }  

        sent += DATAS_LENGTH;
        rest -= DATAS_LENGTH;

        if (rest >= READ_BIN_BUFFER_SIZE) 
        {
            DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
        }
        else 
        {
            //��Ϊ��дFLASH��ʱ����1024�ˣ�����������Ҫд1024���ֽ�
            DATAS_LENGTH = READ_BIN_BUFFER_SIZE;
//            DATAS_LENGTH = rest; //ֻдʣ������
        }

        flash_addr += DATAS_LENGTH;        
    } 
    
    return 1;    
}

int getFileLength(char *src ,char *pcBuf, char *pcRes)
{
	char* pcBegin = NULL;
	char* pcEnd = NULL;
    int tmplen = strlen(pcBuf);
    char strLen[32] = {0};
	
	pcBegin = strstr(src, pcBuf);
//	printf("pcbegin = %s\r\n",pcBegin);

	if (pcBegin == NULL)
	{
		printf("pcBegin is NULL!\n");
		return -1;
	}

	pcEnd = strstr(pcBegin, pcRes);
//	printf("pcEnd = %s\r\n", pcEnd);

	if (pcEnd == NULL)
	{
		printf("pcEnd is NULL!\n");
		return-1;
	}

	if (pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
	{
		printf("not found!\n");
	}
	else
	{		
		memcpy(strLen, pcBegin+tmplen, pcEnd - pcBegin-tmplen);
	}

	return atoi(strLen);
}
int getFileBegin(char* src, char* pcBuf)
{
	char* pcBegin = NULL;	
	int tmplen = strlen(pcBuf);	

	pcBegin = strstr(src, pcBuf);
//	printf("pcbegin = %s\r\n", pcBegin);

	if (pcBegin == NULL)
	{
		printf("pcBegin is NULL!\n");
		return -1;
	}

	return pcBegin - src + tmplen;
}


#if 1

static char HttpConnect(void)
{
    uint32_t cnt = 0;
    uint32_t sum = 0;	
    int recelen = 0;
    char* host_ip;
    char debug_cnt[16] = {0};
    
	struct sockaddr_in client_addr;
    
    uint32_t timeout = 3*60*1000;


#ifdef LWIP_DNS
            struct ip_addr dns_ip;
            netconn_gethostbyname ( HOST_NAME, &dns_ip );
            host_ip = ip_ntoa ( &dns_ip );
            printf ( "host name : %s , host_ip : %s\n",HOST_NAME,host_ip );
#else
            host_ip = HOST_NAME;
#endif  


    while(1)
    {
        //���ó�ʱʱ��
        if((xTaskGetTickCount() - timeout) <= 0)
        {  
            return -1;
        }
    
        sock = socket ( AF_INET, SOCK_STREAM, 0 );
        if ( sock < 0 )
        {
            DBG ( "Socket error\n" );
            vTaskDelay ( 10 );
            continue;
        }  
        
        DBG( "Socket success\n" );
        
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons ( PORT );
        client_addr.sin_addr.s_addr = inet_addr ( host_ip );
        memset ( & ( client_addr.sin_zero ), 0, sizeof ( client_addr.sin_zero ) );
        
        if ( connect ( sock,
                       ( struct sockaddr* ) &client_addr,
                       sizeof ( struct sockaddr ) ) == -1 )
        {
            DBG ( "Connect failed!\n" );
            closesocket ( sock );
            vTaskDelay ( 10 );
            continue;
        }                   
        
        DBG ( "Connect to server successful!\n" );  

         return CONNECT_OK;

    }

    return CONNECT_OK;
}



#endif



#if 0
#define send_duration	20	//�¶ȷ������ڣ�ms��

float temp = 0;
float humid = 0;

static uint32_t send_cnt = 0;
static uint32_t recv_cnt = 0;


void HttpUpgrade(void)
{	
	int32_t rc = 0;
	unsigned char buf[1024];    
	uint32_t len = 0;    
	uint8_t connect_flag = 0;		//���ӱ�־
	uint8_t msgtypes = CONNECT;		//��Ϣ״̬��ʼ��
	
    //��ȡ��ǰ�δ���Ϊ��������ʼʱ��
	uint32_t curtick  =	 xTaskGetTickCount();
	uint32_t sendtick =  xTaskGetTickCount();

    uint32_t cnt = 0;
    uint32_t sum = 0;
	int sock = -1,
    int recelen = 0;
    char* host_ip;
    char debug_cnt[16] = {0};
    char sendbuf[512] = {0};
    uint8_t recvbuff[MAX_RECV_LEN] = {0};
	struct sockaddr_in client_addr;


#ifdef LWIP_DNS
        struct ip_addr dns_ip;
        netconn_gethostbyname ( HOST_NAME, &dns_ip );
        host_ip = ip_ntoa ( &dns_ip );
        DBG( "host name : %s , host_ip : %s\n",HOST_NAME,host_ip );
#else
        host_ip = HOST_NAME;
#endif    
        //http get�����
        GetHTTPGETString(sendbuf);


	while(1)
	{
        sock = socket ( AF_INET, SOCK_STREAM, 0 );
        if ( sock < 0 )
        {
            DBG ( "Socket error\n" );
            vTaskDelay ( 10 );
            continue;;
        }
        
        DBG( "Socket success\n" );
        
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons ( PORT );
        client_addr.sin_addr.s_addr = inet_addr ( host_ip );
        memset ( & ( client_addr.sin_zero ), 0, sizeof ( client_addr.sin_zero ) );
        
        if ( connect ( sock,
                       ( struct sockaddr* ) &client_addr,
                       sizeof ( struct sockaddr ) ) == -1 )
        {
            DBG ( "Connect failed!\n" );
            closesocket ( sock );
            vTaskDelay ( 10 );
            continue;
        }                   
        
        DBG ( "Connect to server successful!\n" );
                       
        memset ( recvbuff,0,sizeof(recvbuff) );  
        write ( sock,sendbuf,strlen(sendbuf));
    
        DBG ( "\r\n***************************************************\r\n" );
        
		if(connect_flag == 1)
		{
			if((xTaskGetTickCount() - sendtick) >= (send_duration*200))
			{
                log_d("send PUBLISH!!  get msgtypes = %d \r\n",msgtypes);
                
				sendtick = xTaskGetTickCount();
                
				taskENTER_CRITICAL();	//�����ٽ���(�޷����жϴ��)
				temp =10.0;
				taskEXIT_CRITICAL();		//�˳��ٽ���(���Ա��жϴ��)
				
				humid = 54.8+rand()%10+1;
//				sprintf((char*)payload_out,"{\"params\":{\"CurrentTemperature\":+%0.1f,\"RelativeHumidity\":%0.1f},\"method\":\"thing.event.property.post\"}",temp, humid);
                sprintf((char*)payload_out,"{\"commandCode\":\"1000\",\"data\":{\"CurrentTemperature\":\"%0.1f\",\"RelativeHumidity\":\"%0.1f\"},\"dev\":\"arm test\"}",temp, humid);

				payload_out_len = strlen((char*)payload_out);
				topicString.cstring = DEVICE_PUBLISH;		//�����ϱ� ����
				log_d("send PUBLISH buff = %s\r\n",payload_out);
				len = MQTTSerialize_publish((unsigned char*)buf, buflen, 0, req_qos, retained, msgid, topicString, payload_out, payload_out_len);//������Ϣ
				rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
				if(rc == len)															//
					log_d("the %dth send PUBLISH Successfully\r\n",send_cnt++);
				else
					log_d("send PUBLISH failed\r\n");  
				log_d("send temp(%0.1f)&humid(%0.1f) !\r\n",temp, humid);
			}
		}
        
		switch(msgtypes)
		{

            //���ӷ���� �ͻ����������ӷ����
            case CONNECT:	len = MQTTSerialize_connect((unsigned char*)buf, buflen, &data); 						//��ȡ�����鳤��		����������Ϣ     
							rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);		//���� ���ط������鳤��
							if(rc == len)															//
								log_d("send CONNECT Successfully\r\n");
							else
								log_d("send CONNECT failed\r\n");               
							log_d("step = %d,MQTT concet to server!\r\n",CONNECT);
							msgtypes = 0;
							break;
           //ȷ����������
			case CONNACK:   if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, (unsigned char*)buf, buflen) != 1 || connack_rc != 0)	//�յ���ִ
							{
								log_d("Unable to connect, return code %d\r\n", connack_rc);		//��ִ��һ�£�����ʧ��
							}
							else
							{
								log_d("step = %d,MQTT is concet OK!\r\n",CONNACK);									//���ӳɹ�
								connect_flag = 1;
							}
							msgtypes = SUBSCRIBE;													//���ӳɹ� ִ�� ���� ����
							break;
            //�������� �ͻ��˶�������
			case SUBSCRIBE: topicString.cstring = DEVICE_SUBSCRIBE;
							len = MQTTSerialize_subscribe((unsigned char*)buf, buflen, 0, msgid, 1, &topicString, &req_qos);
							rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
							if(rc == len)
								log_d("send SUBSCRIBE Successfully\r\n");
							else
							{
								log_d("send SUBSCRIBE failed\r\n"); 
								t++;
								if(t >= 10)
								{
									t = 0;
									msgtypes = CONNECT;
								}
								else
									msgtypes = SUBSCRIBE;
								break;
							}
							log_d("step = %d,client subscribe:[%s]\r\n",SUBSCRIBE,topicString.cstring);
							msgtypes = 0;
							break;
            //����ȷ�� ����������ȷ��
			case SUBACK: 	rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, (unsigned char*)buf, buflen);	//�л�ִ  QoS                                                     
							log_d("step = %d,granted qos is %d\r\n",SUBACK, granted_qos);         								//��ӡ Qos                                                       
							msgtypes = 0;
							break;
            //������Ϣ
			case PUBLISH:	rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,&payload_in, &payloadlen_in, (unsigned char*)buf, buflen);	//��ȡ������������Ϣ
							log_d("step = %d,message arrived : %s\r\n",PUBLISH,payload_in);
							cJSON *json , *json_params, *json_id, *json_led, *json_display;
							json = cJSON_Parse((char *)payload_in);			//�������ݰ�
							if (!json)  
							{  
								log_d("Error before: [%s]\r\n",cJSON_GetErrorPtr());  
							} 
							else
							{
								json_id = cJSON_GetObjectItem(json , "commandCode"); 
								if(json_id->type == cJSON_String)
								{
									log_d("commandCode:%s\r\n", json_id->valuestring);  
								}
								json_params = cJSON_GetObjectItem( json , "data" );

                                json_led = cJSON_GetObjectItem( json_params , "userId" );

                                json_display = cJSON_GetObjectItem( json_params , "startTime" );
  
                			  log_d("code:%s ;   userId:%d ;   startTime:%s ,qos = %d\r\n",
                              json_id->valuestring,
                              json_led->valueint,
                              json_display->valuestring,
                              qos);
                              
                								 
							}
							cJSON_Delete(json);
                            cJSON_Delete(json_params);
                            cJSON_Delete(json_id);
                            cJSON_Delete(json_display);
                            log_d("the %dth recv \r\n",recv_cnt++);
							
							if(qos == 1)
							{
								log_d("publish qos is 1,send publish ack.\r\n");							//QosΪ1�����л�ִ ��Ӧ
								memset(buf,0,buflen);
								len = MQTTSerialize_ack((unsigned char*)buf,buflen,PUBACK,dup,msgid);   					//publish ack                       
								rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);			//
								if(rc == len)
									log_d("send PUBACK Successfully\r\n");
								else
									log_d("send PUBACK failed\r\n");                                       
							}
							msgtypes = 0;
							break;
            //����ȷ�� QoS	1��Ϣ�����յ�ȷ��
			case PUBACK:    log_d("step = %d,PUBACK!\r\n",PUBACK);					//�����ɹ�
							msgtypes = 0;
							break;
            //�����յ� QoS2 ��һ��
			case PUBREC:    log_d("step = %d,PUBREC!\r\n",PUBREC);     				//just for qos2
							break;
            //�����ͷ� QoS2 �ڶ���
			case PUBREL:    log_d("step = %d,PUBREL!\r\n",PUBREL);        			//just for qos2
							break;
            //������� QoS2 ������                            
			case PUBCOMP:   log_d("step = %d,PUBCOMP!\r\n",PUBCOMP);        			//just for qos2
							break;
            //��������
			case PINGREQ:   len = MQTTSerialize_pingreq((unsigned char*)buf, buflen);							//����
							rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
							if(rc == len)
								log_d("send PINGREQ Successfully\r\n");
							else
								log_d("send PINGREQ failed\r\n");       
								log_d("step = %d,time to ping mqtt server to take alive!\r\n",PINGREQ);
								msgtypes = 0;
							break;
            //������Ӧ
			case PINGRESP:	log_d("step = %d,mqtt server Pong\r\n",PINGRESP);  			//������ִ����������Ӧ                                                     
							msgtypes = 0;
							break;
			default:
							break;

		}
		memset(buf,0,buflen);
		rc=MQTTPacket_read((unsigned char*)buf, buflen, transport_getdata);//��ѯ����MQTT�������ݣ�
		log_d("MQTTPacket_read = %d\r\n",rc);		
		if(rc >0)//��������ݣ�������Ӧ״̬��
		{
			msgtypes = rc;
			log_d("MQTT is get recv: msgtypes = %d\r\n",msgtypes);
		}
	}
	transport_close(mysock);
    log_d("mqtt thread exit.\r\n");
}

#endif


void ClearBinQueue(BINQUEUE_T queue)
{
    memset(queue->data,0x00,sizeof(queue->data));
    queue->Pread = 0;
    queue->Pwrite= 0;
    queue->Count=0;
}

int WriteBinQueue(BINQUEUE_T queue,uint8_t *buf,uint16_t length)
{
    if (queue->Count + length > QUEUE_MAX_LEN)//д������ݳ���������
    {
        printf("error:write over\r\n");
        return WRITEOVER;//д����Խ��
    }
    while (length--)
    {
        queue->data[queue->Pwrite] = *buf;//��ֵ��������
        buf++;//��������ַ��һ        
        queue->Count++;
        queue->Pwrite++;//
        if (queue->Pwrite == QUEUE_MAX_LEN)
        {
            queue->Pwrite = 0;
        }

    }
    return OK;//���ݶ�ȡ�ɹ�

}
int ReadBinQueue(BINQUEUE_T queue,uint8_t *buf,uint16_t length)
{
    if (queue->Count - length < 0)//������û���㹻������
    {
        printf("error:read over\r\n");
        return READOVER;//������Խ��
    }
    while (length--)
    {
        *buf = queue->data[queue->Pread];
        buf++;
        queue->Count--;
        queue->Pread++;//��ȡָ���Լ�
        if (queue->Pread == QUEUE_MAX_LEN)
        {
            queue->Pread = 0;
        }

    }
    return OK;//���ݶ�ȡ�ɹ�

}




