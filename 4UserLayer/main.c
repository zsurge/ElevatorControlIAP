#include "def.h"
#include "mqtt_app.h"
#include "client.h"
#include "pub_options.h"

#define LOG_TAG    "main"
#include "elog.h"

//�������ȼ� 
#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY)
#define MQTT_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)

//�����ջ��С	

#define LED_STK_SIZE 		1024
#define UPDATE_STK_SIZE 	1024*1
#define MQTT_STK_SIZE        (3*1024)


#define W_SPI_FLASH_OK      "5555"   //APP д��SPI FLASH OK
#define W_MCU_FLASH_OK      "6666"   //APP д��MCU FLASH OK
#define W_SPI_FLASH_NEED    "5050"   //��Ҫд��SPI FLASH
#define W_MCU_FLASH_NEED    "6060"   //��Ҫд��MCU_FLASH


//������
TaskHandle_t xHandleTaskMqtt = NULL;      //MQTT ����


//������
static void vTaskUpdate(void *pvParameters);
static void vTaskLed(void *pvParameters);
static void EasyLogInit(void);
static void vTaskMqttTest(void *pvParameters);
static void AppTaskCreate(void);


int main(void)
{ 
	bsp_Init();  
    EasyLogInit();  

//    ef_env_set_default();

	/* �������� */
	AppTaskCreate();
    vTaskStartScheduler();          //�����������            

}


static void AppTaskCreate (void)
{

    StartEthernet();  

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

    char spi_flag[4+1] = {0};
    char mcu_flag[4+1] = {0};    

    int8_t spi_flag_len = 0;
    int8_t mcu_flag_len = 0;  


    while(1)
        {   

            //��ӡ�²�����DEBUGʱʹ�ã�RELEASE����ɾ��
//            ef_print_env();
            ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
    
            //��ȡ������־λ
            spi_flag_len = ef_get_env_blob("WSPIFLASH", spi_flag, sizeof(spi_flag) , NULL);
            mcu_flag_len = ef_get_env_blob("WMCUFLASH", mcu_flag, sizeof(mcu_flag) , NULL);

            log_d("%s,%s,%d,%d\r\n",spi_flag,mcu_flag,spi_flag_len,mcu_flag_len);            

            spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flag);

            log_d("spi_flash_flag = %d\r\n",spi_flash_flag);
    
            mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flag); 
            
            log_d("mcu_flash_flag = %d\r\n",mcu_flash_flag);
            
            //������ת
            if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
            {
                log_e("jump app \r\n");
                
                //����Ҫ����������APP
                if(IAP_JumpToApplication())
                {
                    log_e("iap jump error,please download app\r\n");

//                    ReadIAP();
    
                    //��תʧ�ܣ�����Ҫ�������б�־λ������дSPI FLASH��MCU FLASH
                    ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                    ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  
    
                    //add 2019.09.05���Է��֣���Ҫ�������������ɹ�
                    NVIC_SystemReset();
                }            
            }
            else
            {
                //�ж�SPI FLASH�Ƿ���д�꣬MCU FLASHδд�꣬��FLASH��дMCU FLASH
                if(spi_flash_flag == 0) //�����Ѵ���flash�ڲ�������δд��mcu
                {
                    log_e("IAP STATR! ---> Write MCU FLASH\r\n");
                    //��ȡ�ļ���С
                    file_size = ef_get_env((const char * )"FileSize");
                    file_total_size = str2int((const char *)file_size);
    
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //д��MCU FLASH ��ɱ�־λ
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            log_e("STM_FLASH_Write success\r\n");
                        } 
                    }
                }
                else //SPI FLASH�޳����ļ����������س����ļ���д�뵽MCU FLASH��
                {     
                    printf("IAP STATR! ---> Write SPI FLASH\r\n");
                    
                    //��Ҫ����������IAP��������
                    file_total_size = IAP_DownLoadToFlash();   
                    
                    if(file_total_size > 0)
                    {
                        if(ef_set_env("WSPIFLASH",W_SPI_FLASH_OK) == EF_NO_ERR)
                        {
                           log_e("IAP_DownLoadToSTMFlash success!\r\n");
                        } 
                                                    
                        log_d("write stm flash\r\n");
                        
                        if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                        {                           
                            //д��MCU FLASH ��ɱ�־λ
                            if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                            {
                                log_e("STM_FLASH_Write success\r\n");

                                ef_set_env("up_status", "101711");

                                NVIC_SystemReset();
                            }  
                        }
                        else
                        {
                            log_e("IAP_DownLoadToSTMFlash error!\r\n");
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

        //��ӡ�²�����DEBUGʱʹ�ã�RELEASE����ɾ��
        //ef_print_env();

        //��ȡ������־λ
        spi_flash_value = ef_get_env("WSPIFLASH");
        spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        
        //������ת
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //����Ҫ����������APP
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //��תʧ�ܣ�����Ҫ�������б�־λ������дSPI FLASH��MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  

                //add 2019.09.05���Է��֣���Ҫ�������������ɹ�
                NVIC_SystemReset();
            }            
        }
        else
        {
            //�ж�SPI FLASH�Ƿ���д�꣬MCU FLASHδд�꣬��FLASH��дMCU FLASH
            if(spi_flash_flag == 0) //�����Ѵ���flash�ڲ�������δд��mcu
            {
                printf("IAP STATR! ---> Write MCU FLASH\r\n");
                //��ȡ�ļ���С
                file_size = ef_get_env((const char * )"FileSize");
                file_total_size = str2int((const char *)file_size);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //д��MCU FLASH ��ɱ�־λ
                    if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                    {
                        printf("STM_FLASH_Write success\r\n");
                    } 
                }
            }
            else //SPI FLASH�޳����ļ����������س����ļ���д�뵽MCU FLASH��
            {     
                printf("IAP STATR! ---> Write SPI FLASH\r\n");
                
                //��Ҫ����������IAP��������
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    printf("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //д��MCU FLASH ��ɱ�־λ
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






