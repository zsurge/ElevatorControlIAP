#include "def.h"
#include "mqtt_app.h"
#include "client.h"

#define LOG_TAG    "main"
#include "elog.h"

//任务优先级 
#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY)
#define MQTT_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)

//任务堆栈大小	

#define LED_STK_SIZE 		1024
#define UPDATE_STK_SIZE 	1024*1
#define MQTT_STK_SIZE        (3*1024)


#define W_SPI_FLASH_OK      "5555"   //APP 写入SPI FLASH OK
#define W_MCU_FLASH_OK      "6666"   //APP 写入MCU FLASH OK
#define W_SPI_FLASH_NEED    "5050"   //需要写入SPI FLASH
#define W_MCU_FLASH_NEED    "6060"   //需要写入MCU_FLASH


//任务句柄
static TaskHandle_t xHandleTaskUpdate = NULL;
static TaskHandle_t xHandleTaskMqtt = NULL;      //MQTT 测试
static TaskHandle_t xHandleTaskLed = NULL;      //LED灯


//任务函数
static void vTaskUpdate(void *pvParameters);
static void vTaskLed(void *pvParameters);
static void EasyLogInit(void);
static void vTaskMqttTest(void *pvParameters);
static void AppTaskCreate(void);


int main(void)
{ 

	bsp_Init();    


    EasyLogInit();  
    
	mymem_init(SRAMIN);								//初始化内部内存池
	mymem_init(SRAMEX);								//初始化外部内存池
	mymem_init(SRAMCCM);	  					    //初始化CCM内存池

	while(lwip_comm_init() != 0) //lwip初始化
	{
        log_d("lwip init error!\r\n");
		delay_ms(1200);
	}

    log_d("lwip init success!\r\n");


	//创建开始任务
//    xTaskCreate((TaskFunction_t )vTaskUpdate,            //任务函数
//                (const char*    )"update",               //任务名称
//                (uint16_t       )UPDATE_STK_SIZE,        //任务堆栈大小
//                (void*          )NULL,                  //传递给任务函数的参数
//                (UBaseType_t    )UPDATE_TASK_PRIO,        //任务优先级
//                (TaskHandle_t*  )&xHandleTaskUpdate);   //任务句柄        



	/* 创建任务 */
	AppTaskCreate();
    vTaskStartScheduler();          //开启任务调度            

}


static void AppTaskCreate (void)
{

#if LWIP_DHCP
                lwip_comm_dhcp_creat();                             //创建DHCP任务
#endif



//    client_init();



    xTaskCreate((TaskFunction_t )vTaskMqttTest,     
                (const char*    )"vMqttTest",   
                (uint16_t       )MQTT_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MQTT_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMqtt);   



}

void vTaskMqttTest(void *pvParameters)
{ 
    uint32_t file_total_size = 0;
    char *spi_flash_value;
    char *mcu_flash_value;
    char *file_size;

    int8_t spi_flash_flag = 0;
    int8_t mcu_flash_flag = 0;  

    while(1)
        {   

            //打印下参数，DEBUG时使用，RELEASE里需删除
//            ef_print_env();
    
            //读取升级标志位
            spi_flash_value = ef_get_env("WSPIFLASH");
            spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);

            log_d("spi_flash_flag = %d\r\n",spi_flash_flag);
    
            mcu_flash_value = ef_get_env("WMCUFLASH"); 
            mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value); 
            
            log_d("mcu_flash_flag = %d\r\n",mcu_flash_flag);
            
            //正常跳转
            if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
            {
                log_d("jump app \r\n");
                
                //不需要升级，进入APP
                if(IAP_JumpToApplication())
                {
                    printf("iap jump error,please download app\r\n");
    
                    //跳转失败，所有要重置所有标志位，升级写SPI FLASH和MCU FLASH
                    ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                    ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  
    
                    //add 2019.09.05测试发现，需要重启才能升级成功
                    NVIC_SystemReset();
                }            
            }
            else
            {
                //判断SPI FLASH是否已写完，MCU FLASH未写完，读FLASH，写MCU FLASH
                if(spi_flash_flag == 0) //程序已存在flash内部，但是未写到mcu
                {
                    printf("IAP STATR! ---> Write MCU FLASH\r\n");
                    //获取文件大小
                    file_size = ef_get_env((const char * )"FileSize");
                    file_total_size = str2int((const char *)file_size);
    
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //写入MCU FLASH 完成标志位
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            printf("STM_FLASH_Write success\r\n");
                        } 
                    }
                }
                else //SPI FLASH无程序文件，重新下载程序文件及写入到MCU FLASH中
                {     
                    printf("IAP STATR! ---> Write SPI FLASH\r\n");
                    
                    //需要升级，进入IAP升级流程
                    file_total_size = IAP_DownLoadToFlash();   
                    
                    if(file_total_size > 0)
                    {
                        if(ef_set_env("WSPIFLASH",W_SPI_FLASH_OK) == EF_NO_ERR)
                        {
                           log_d("IAP_DownLoadToSTMFlash success!\r\n");
                        } 
                                                    
                        log_d("write stm flash\r\n");
                        
                        if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                        {                           
                            //写入MCU FLASH 完成标志位
                            if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                            {
                                log_d("STM_FLASH_Write success\r\n");

                                ef_set_env("up_status", "101711");

//                                NVIC_SystemReset();
                            }  
                        }
                        else
                        {
                            log_d("IAP_DownLoadToSTMFlash error!\r\n");
                            ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                        
                        }
                    }
                }
            }        
    
            vTaskDelay(100);
        }


} 


#if 0

static void vTaskUpdate(void *pvParameters)
{
    uint32_t file_total_size = 0;
    char *spi_flash_value;
    char *mcu_flash_value;
    char *file_size;

    int8_t spi_flash_flag = 0;
    int8_t mcu_flash_flag = 0;

    while(1)
    {   

        //打印下参数，DEBUG时使用，RELEASE里需删除
        //ef_print_env();

        //读取升级标志位
        spi_flash_value = ef_get_env("WSPIFLASH");
        spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        
        //正常跳转
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //不需要升级，进入APP
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //跳转失败，所有要重置所有标志位，升级写SPI FLASH和MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  

                //add 2019.09.05测试发现，需要重启才能升级成功
                NVIC_SystemReset();
            }            
        }
        else
        {
            //判断SPI FLASH是否已写完，MCU FLASH未写完，读FLASH，写MCU FLASH
            if(spi_flash_flag == 0) //程序已存在flash内部，但是未写到mcu
            {
                printf("IAP STATR! ---> Write MCU FLASH\r\n");
                //获取文件大小
                file_size = ef_get_env((const char * )"FileSize");
                file_total_size = str2int((const char *)file_size);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //写入MCU FLASH 完成标志位
                    if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                    {
                        printf("STM_FLASH_Write success\r\n");
                    } 
                }
            }
            else //SPI FLASH无程序文件，重新下载程序文件及写入到MCU FLASH中
            {     
                printf("IAP STATR! ---> Write SPI FLASH\r\n");
                
                //需要升级，进入IAP升级流程
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    printf("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //写入MCU FLASH 完成标志位
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            printf("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                        
                    }
                }
            }
        }        

        vTaskDelay(100);
    }
}
#endif

static void EasyLogInit(void)
{
    /* initialize EasyLogger */
     elog_init();
     /* set EasyLogger log format */
     elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
     elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG );
     elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG );
     elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG );
     elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_TIME);
     elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_TIME);

     
     /* start EasyLogger */
     elog_start();  
}






