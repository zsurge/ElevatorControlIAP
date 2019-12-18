/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : comm.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月18日
  最近修改   :
  功能描述   : 串口通讯协议解析及处理
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月18日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __COMM_H
#define __COMM_H

#include "tool.h"
#include "cJSON.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "errorcode.h"






/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//#define STX		    					0x02	/* frame head */
#define ETX                             0x03    /* frame tail */
#define CMDSUCC	    					0x06	/* command success */
#define CMDERRO	    					0x15	/* command error */
#define UPDAT							0x13	/* Upload data */
    
    
#define SPACE		        			0x00
#define FINISH		       	 			0x55

#define GETSENSOR                       0xA1
#define SETLED                          0xA2
#define GETDEVICEINFO                   0xA3
#define GETVER                          0xA4
#define DEVRESET                        0xA5
#define SETDEVPARAM                     0xA6
#define UPGRADE                         0xA7
#define CONTROLMOTOR                    0xA8

#define WGREADER                        0xA9
#define QRREADER                        0xA0

#define REQUEST_OPEN_DOOR_B             0xB7
#define DOOR_B                          0xB8

#define ERRORINFO                       0XB0

#define HANDSHAKE                       0XB1

#define FIREFIGHTINGLINKAGE             0xB2
#define MANUALLY_OPEN_DOOR_A            0xB3
#define MANUALLY_OPEN_DOOR_B            0xB4



#define DEV_MOTOR                       0x01
#define DEV_SENSOR                      0x02
#define DEV_HEARTBEAT                   0x03



#define JSON_PACK_MAX                   600

#define MAX_RXD_BUF_LEN        			512
#define MAX_TXD_BUF_LEN					512   
#define MAX_CMD_BUF_LEN					256  

//#define USEQUEUE

#ifdef USEQUEUE
#define  QUEUE_LEN    20     /* 队列的长度，最大可包含多少个消息 */
#define QUEUE_BUF_LEN 8 //每个队列buff的长度
typedef struct
{
    uint8_t cmd;                         //指令字
    uint8_t data[QUEUE_BUF_LEN];         //需要发送给android板的数据
}QUEUE_TO_HOST_T;

extern QUEUE_TO_HOST_T gQueueToHost;    //定义一个结构体用于消息队列，跟andorid通信
#endif



//typedef enum
//{
//    NoCMD = 0xA0,
//    GetSensor = 0xA1,
//    SetLed,
//    GetDeviceStatus,
//    GetVersion,
//    UpGradeApp    
//}CommandType;    

typedef struct
{
    uint8_t cmd;     //指令字
    uint8_t *cmd_desc;//指令描述    
    uint8_t cmd_data[MAX_CMD_BUF_LEN];//指令内容(若有)
}CMD_RX_T;

//static CMD_RX_T gcmd_rx;

typedef struct
{
    uint8_t cmd;         //指令字
    uint8_t code;        //执行状态   
    uint8_t data[MAX_CMD_BUF_LEN];       //指令返回内容(若有)
}CMD_TX_T;

typedef struct
{
    uint8_t RxdStatus;                 //接收状态
    uint8_t RxCRCHi;                   //校验值高8位
    uint8_t RxCRCLo;                   //校验值低8位
    uint8_t RxdFrameStatus;            //接收包状态
    uint16_t NowLen;                    //接收字节数
    uint16_t RxdTotalLen;               //接收包数据长度
    volatile uint8_t RxdBuf[MAX_RXD_BUF_LEN];   //接收包数据缓存
}RECVHOST_T;






static RECVHOST_T gRecvHost;

//static CMD_TX_T gcmd_tx;

////声明互斥信号量
extern SemaphoreHandle_t  gxMutex;

//声明二值信号量
//extern SemaphoreHandle_t gBinarySem_Handle;



extern TaskHandle_t xHandleTaskQueryMotor;      //电机状态查询



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void deal_rx_data(void);
void deal_Serial_Parse(void);

void init_serial_boot(void);
SYSERRORCODE_E send_to_host(uint8_t cmd,uint8_t *buf,uint8_t len);
void send_to_device(CMD_RX_T *cmd_rx);
SYSERRORCODE_E SendAsciiCodeToHost(uint8_t cmd,SYSERRORCODE_E code,uint8_t *buf);


#endif

