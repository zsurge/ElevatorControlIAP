/**********************************************************************************************************
** 文件名		:mqtt_app.c
** 作者			:maxlicheng<licheng.chn@outlook.com>
** 作者github	:https://github.com/maxlicheng
** 作者博客		:https://www.maxlicheng.com/	
** 生成日期		:2018-08-08
** 描述			:mqtt服务程序
************************************************************************************************************/
#include "mqtt_app.h"
#include "MQTTPacket.h"
#include "transport.h"
#include "FreeRTOS.h"
#include "task.h"
#include "malloc.h"
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#define LOG_TAG    "MQTTAPP"
#include "elog.h"


#define send_duration	20	//温度发送周期（ms）

float temp = 0;
float humid = 0;

static uint32_t send_cnt = 0;
static uint32_t recv_cnt = 0;


void mqtt_thread(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	MQTTString receivedTopic;
	MQTTString topicString = MQTTString_initializer;
	
	int32_t rc = 0;
	unsigned char buf[1024];
	int buflen = sizeof(buf);
	int	mysock = 0;
	
	int payloadlen_in;
	unsigned char* payload_in;
	unsigned short msgid = 1;
	int subcount;
	int granted_qos =0;
	unsigned char sessionPresent, connack_rc;
	unsigned short submsgid;
	uint32_t len = 0;
	int req_qos = 0;
	unsigned char dup;
	int qos;
	unsigned char retained = 0;

	uint8_t connect_flag = 0;		//连接标志
	uint8_t msgtypes = CONNECT;		//消息状态初始化
	uint8_t t=0;
	
	log_d("socket connect to server\r\n");
	mysock = transport_open((char *)HOST_NAME,HOST_PORT);
	log_d("Sending to hostname %s port %d\r\n", HOST_NAME, HOST_PORT);
	
	len = MQTTSerialize_disconnect((unsigned char*)buf,buflen);
	rc = transport_sendPacketBuffer(mysock, (uint8_t *)buf, len);
	if(rc == len)															//
		log_d("send DISCONNECT Successfully\r\n");
	else
		log_d("send DISCONNECT failed\r\n"); 
    
	vTaskDelay(2500);
	
	log_d("socket connect to server\r\n");
	mysock = transport_open((char *)HOST_NAME,HOST_PORT);
	log_d("Sending to hostname %s port %d\r\n", HOST_NAME, HOST_PORT);


	data.clientID.cstring = CLIENT_ID;                   //随机
	data.keepAliveInterval = KEEPLIVE_TIME;         //保持活跃
	data.username.cstring = USER_NAME;              //用户名
	data.password.cstring = PASSWORD;               //秘钥
	data.MQTTVersion = MQTT_VERSION;                //3表示3.1版本，4表示3.11版本
	data.cleansession = 1;
    
	unsigned char payload_out[200];
	int payload_out_len = 0;

    //获取当前滴答，作为心跳包起始时间
	uint32_t curtick  =	 xTaskGetTickCount();
	uint32_t sendtick =  xTaskGetTickCount();
    
	while(1)
	{
		if(( xTaskGetTickCount() - curtick) >(data.keepAliveInterval*200))		//每秒200次tick
		{
			if(msgtypes == 0)
			{                
				curtick =  xTaskGetTickCount();
				msgtypes = PINGREQ;
                log_d("send heartbeat!!  set msgtypes = %d \r\n",msgtypes);
			}

		}
		if(connect_flag == 1)
		{
			if((xTaskGetTickCount() - sendtick) >= (send_duration*200))
			{
                log_d("send PUBLISH!!  get msgtypes = %d \r\n",msgtypes);
                
				sendtick = xTaskGetTickCount();
                
				taskENTER_CRITICAL();	//进入临界区(无法被中断打断)
				temp =10.0;
				taskEXIT_CRITICAL();		//退出临界区(可以被中断打断)
				
				humid = 54.8+rand()%10+1;
//				sprintf((char*)payload_out,"{\"params\":{\"CurrentTemperature\":+%0.1f,\"RelativeHumidity\":%0.1f},\"method\":\"thing.event.property.post\"}",temp, humid);
                sprintf((char*)payload_out,"{\"commandCode\":\"1000\",\"data\":{\"CurrentTemperature\":\"%0.1f\",\"RelativeHumidity\":\"%0.1f\"},\"dev\":\"arm test\"}",temp, humid);

				payload_out_len = strlen((char*)payload_out);
				topicString.cstring = DEVICE_PUBLISH;		//属性上报 发布
				log_d("send PUBLISH buff = %s\r\n",payload_out);
				len = MQTTSerialize_publish((unsigned char*)buf, buflen, 0, req_qos, retained, msgid, topicString, payload_out, payload_out_len);//发布消息
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

            //连接服务端 客户端请求连接服务端
            case CONNECT:	len = MQTTSerialize_connect((unsigned char*)buf, buflen, &data); 						//获取数据组长度		发送连接信息     
							rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);		//发送 返回发送数组长度
							if(rc == len)															//
								log_d("send CONNECT Successfully\r\n");
							else
								log_d("send CONNECT failed\r\n");               
							log_d("step = %d,MQTT concet to server!\r\n",CONNECT);
							msgtypes = 0;
							break;
           //确认连接请求
			case CONNACK:   if(MQTTDeserialize_connack(&sessionPresent, &connack_rc, (unsigned char*)buf, buflen) != 1 || connack_rc != 0)	//收到回执
							{
								log_d("Unable to connect, return code %d\r\n", connack_rc);		//回执不一致，连接失败
							}
							else
							{
								log_d("step = %d,MQTT is concet OK!\r\n",CONNACK);									//连接成功
								connect_flag = 1;
							}
							msgtypes = SUBSCRIBE;													//连接成功 执行 订阅 操作
							break;
            //订阅主题 客户端订阅请求
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
            //订阅确认 订阅请求报文确认
			case SUBACK: 	rc = MQTTDeserialize_suback(&submsgid, 1, &subcount, &granted_qos, (unsigned char*)buf, buflen);	//有回执  QoS                                                     
							log_d("step = %d,granted qos is %d\r\n",SUBACK, granted_qos);         								//打印 Qos                                                       
							msgtypes = 0;
							break;
            //发布消息
			case PUBLISH:	rc = MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,&payload_in, &payloadlen_in, (unsigned char*)buf, buflen);	//读取服务器推送信息
							log_d("step = %d,message arrived : %s\r\n",PUBLISH,payload_in);
							cJSON *json , *json_params, *json_id, *json_led, *json_display;
							json = cJSON_Parse((char *)payload_in);			//解析数据包
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
								log_d("publish qos is 1,send publish ack.\r\n");							//Qos为1，进行回执 响应
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
            //发布确认 QoS	1消息发布收到确认
			case PUBACK:    log_d("step = %d,PUBACK!\r\n",PUBACK);					//发布成功
							msgtypes = 0;
							break;
            //发布收到 QoS2 第一步
			case PUBREC:    log_d("step = %d,PUBREC!\r\n",PUBREC);     				//just for qos2
							break;
            //发布释放 QoS2 第二步
			case PUBREL:    log_d("step = %d,PUBREL!\r\n",PUBREL);        			//just for qos2
							break;
            //发布完成 QoS2 第三步                            
			case PUBCOMP:   log_d("step = %d,PUBCOMP!\r\n",PUBCOMP);        			//just for qos2
							break;
            //心跳请求
			case PINGREQ:   len = MQTTSerialize_pingreq((unsigned char*)buf, buflen);							//心跳
							rc = transport_sendPacketBuffer(mysock, (unsigned char*)buf, len);
							if(rc == len)
								log_d("send PINGREQ Successfully\r\n");
							else
								log_d("send PINGREQ failed\r\n");       
								log_d("step = %d,time to ping mqtt server to take alive!\r\n",PINGREQ);
								msgtypes = 0;
							break;
            //心跳响应
			case PINGRESP:	log_d("step = %d,mqtt server Pong\r\n",PINGRESP);  			//心跳回执，服务有响应                                                     
							msgtypes = 0;
							break;
			default:
							break;

		}
		memset(buf,0,buflen);
		rc=MQTTPacket_read((unsigned char*)buf, buflen, transport_getdata);//轮询，读MQTT返回数据，
		log_d("MQTTPacket_read = %d\r\n",rc);		
		if(rc >0)//如果有数据，进入相应状态。
		{
			msgtypes = rc;
			log_d("MQTT is get recv: msgtypes = %d\r\n",msgtypes);
		}
	}
	transport_close(mysock);
    log_d("mqtt thread exit.\r\n");
}



/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
// [IN] pbSrc - 输入16进制数的起始地址
// [IN] nLen - 16进制数的字节数
// return value: 
// remarks : 将16进制数转化为字符串
*/
void HexToStr(uint8_t *pbDest, uint8_t *pbSrc, int nLen)
{
	char ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	pbDest[nLen*2] = '\0';
}



