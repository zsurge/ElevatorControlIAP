/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : ini.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��28��
  ����޸�   :
  ��������   : ��������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "ini.h"
#include "version.h"
#include "string.h"
#include "bsp_uart_fifo.h"

ICREADER_T   gICReaderParam; 
QRCODE_T     gQRCodeParam;

static SYSERRORCODE_E SaveICParam(void);
static SYSERRORCODE_E SaveQRParam(void);



#define SFUD_DEMO_TEST_BUFFER_SIZE 1024

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];


void ReadIAP(void) 
{
    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    uint32_t addr = 0x300000;
    size_t size = SFUD_DEMO_TEST_BUFFER_SIZE;
    size_t j = 0;

    for(j=0;j<42;j++,addr+=SFUD_DEMO_TEST_BUFFER_SIZE)
    {
        
        result = sfud_read(flash, addr, size, sfud_demo_test_buf);
        if (result == SFUD_SUCCESS) 
        {
            printf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\r\n", flash->name, addr,
                    size);
            printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
            for (i = 0; i < size; i++) 
            {
                if (i % 16 == 0) 
                {
                    printf("[%08X] ", addr + i);
                }
                printf("%02X ", sfud_demo_test_buf[i]);
                if (((i + 1) % 16 == 0) || i == size - 1) 
                {
                    printf("\r\n");
                }
            }
            printf("\r\n");
        } 
        else 
        {
            printf("Read the %s flash data failed.\r\n", flash->name);
        }
    }
    if (i == size) {
        printf("The %s flash test is success.\r\n", flash->name);
    }
}


//#define param_start_addr 0x00

/*****************************************************************************
 �� �� ��  : RestoreDefaultSetting
 ��������  : �ָ�Ĭ������,ʵ����д��ϵͳ��������Ҫ�Ĳ���
           ��ʹ��ef_env_set_default ����Ҫ��ef_port.c�ж�Ĭ��ֵ�����޸�
 �������  : 
             
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�

    �޸�����   : �����ɺ���

*****************************************************************************/
void RestoreDefaultSetting(void)
{
    if(ef_env_set_default()== EF_NO_ERR)
    {
        DBG("RestoreDefaultSetting success\r\n");
    }
    
}

void SystemUpdate(void)
{
    //д������־λ
    if(ef_set_env("WSPIFLASH","5050") == EF_NO_ERR)
    {
        ef_set_env("WMCUFLASH","6060");
        //jump iap
        DBG("jump iap\r\n");
        NVIC_SystemReset();
    }
}

SYSERRORCODE_E RecordBootTimes(void)
{
    SYSERRORCODE_E result = NO_ERR;
    
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[11] = {0};


    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);

    DBG("boot_times = %ld\r\n",i_boot_times);
    
    /* boot count +1 */
    i_boot_times ++;

    /* interger to string */
    sprintf(c_new_boot_times,"%ld", i_boot_times);
    
    /* set and store the boot count number to Env */
    if(ef_set_env("boot_times", c_new_boot_times) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    }
    
    return result;
}

static SYSERRORCODE_E SaveICParam(void)
{
    SYSERRORCODE_E result = NO_ERR;
    char CurrentICstate[4] = {0};      

    sprintf(CurrentICstate,"%04d", gICReaderParam.FunState);

    DBG("CurrentICstate = %s\r\n",CurrentICstate);

    if(ef_set_env("ICSTATE", CurrentICstate) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    }
    
    return result;

}

static SYSERRORCODE_E SaveQRParam(void)
{
    SYSERRORCODE_E result = NO_ERR;
    
    char CurrentQRstate[4] = {0};  
    char CurrentQRLightMode[4] = {0}; 
    char CurrentQRScanMode[4] = {0}; 
    char CurrentQRTimeInterval[4] = {0}; 

    uint8_t Configuration[8] = {0x02,0x39,0x39,0x39,0x39,0x39,0x39,0x03};

    uint8_t LightOn[8] = {0x02, 0x31,0x32,0x36,0x30,0x31,0x35 ,0x03};//�򿪲����
    uint8_t LightOff[8] = { 0x02,0x31,0x32,0x36,0x30,0x30,0x35,0x03 };//�رղ����
    
    uint8_t AlwaysScan[8] = { 0x02,0x38,0x31,0x38,0x39,0x30,0x30,0x03 }; //��ͬ����û��ʱ����
    uint8_t SingleScan[8] = { 0x02,0x31,0x31,0x38,0x39,0x31,0x37,0x03 }; //��ʶ����ͬ���룬����ʶ��һ��
    
//    uint8_t Timeout1Sec[9] = { 0x02,0x38,0x31,0x38,0x39,0x32,0x30,0x30,0x03 };//��ͬ����1����ʱ
//    uint8_t Timeout2Sec[9] = { 0x02,0x38,0x31,0x38,0x39,0x34,0x30,0x30,0x03 };//��ͬ����2����ʱ
    uint8_t Timeout600mSec[9] = { 0x02,0x38,0x31,0x38,0x39,0x31,0x32,0x30,0x03 };//��ͬ����600ms����ʱ

    //QR����ʹ��-��ֹ
    sprintf(CurrentQRstate,"%04d", gQRCodeParam.FunState);      
    if(ef_set_env("QRSTATE", CurrentQRstate) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    }

    DBG("CurrentQRstate = %s\r\n",CurrentQRstate);


    //��������ģʽ
    comSendBuf(COM3,Configuration,8);
     
    //���õƹ�ģʽ    
    if(gQRCodeParam.LightMode == 0)
    {
        comSendBuf(COM3,LightOff,8); 
    }
    else
    {
        comSendBuf(COM3,LightOn,8); 
    }
        
    sprintf(CurrentQRLightMode,"%04d", gQRCodeParam.LightMode);
     DBG("CurrentQRLightMode = %s\r\n",CurrentQRLightMode);

    if(ef_set_env("LIGHTMODE", CurrentQRLightMode) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    }

    //���� ɨ��ģʽ 
    if(gQRCodeParam.ScanMode == 0)
    {
        comSendBuf(COM3,AlwaysScan,8); 
    }
    else
    {
        comSendBuf(COM3,SingleScan,8); 
    }

    sprintf(CurrentQRScanMode,"%04d", gQRCodeParam.ScanMode);
         DBG("CurrentQRScanMode = %s\r\n",CurrentQRScanMode);
    

    if(ef_set_env("SCANMODE", CurrentQRScanMode) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    }

    //����ɨ��ʱ���� �豸�����⣬��ʱ������
//    if(gQRCodeParam.TimeInterval == 1)
//    {
//        comSendBuf(COM3,Timeout1Sec,9); 
//    }
//    else if(gQRCodeParam.TimeInterval == 2)
//    {
//        comSendBuf(COM3,Timeout2Sec,9); 
//    }
//    else
//    {
        comSendBuf(COM3,Timeout600mSec,9); 
//    }    

    //�ر�����ģʽ
    comSendBuf(COM3,Configuration,8);

    sprintf(CurrentQRTimeInterval,"%04d", gQRCodeParam.TimeInterval);
    DBG("CurrentQRTimeInterval = %s\r\n",CurrentQRTimeInterval);

    

    if(ef_set_env("TIMEINTERVAL", CurrentQRTimeInterval) != EF_NO_ERR)
    {
        result = FLASH_W_ERR;  
    } 
    
    return result;
}



SYSERRORCODE_E ParseDevParam(uint8_t *ParamBuff)
{
    SYSERRORCODE_E result = NO_ERR;
    uint8_t buff[6] = {0};

    if(!ParamBuff)
    {
        DBG("param error!\r\n");
        return CJSON_PARSE_ERR;  
    }

    memcpy(buff,ParamBuff,5);    

     if(buff[4] == 0x00)
    {
        //��ֹIC��
        gICReaderParam.FunState = 0;
    }
    else
    {
        //ʹ��IC��������Ĭ��ֵ
        gICReaderParam.FunState = 1;
    }   

    DBG("gICReaderParam.FunState = %d\r\n",gICReaderParam.FunState);

    gICReaderParam.SaveICParam = SaveICParam;
    gICReaderParam.SaveICParam();

    if(buff[3] == 0)
    {
        //��ֹQR CODE
        gQRCodeParam.FunState = 0;
    }
    else
    {
        //ʹ��QR CDOE ������Ĭ��ֵ
        gQRCodeParam.FunState = 1;        
    } 

    DBG("gQRCodeParam.FunState = %d\r\n",gQRCodeParam.FunState);

    gQRCodeParam.LightMode = buff[2];
    gQRCodeParam.ScanMode = buff[1];
    gQRCodeParam.TimeInterval = buff[0];

    DBG("gQRCodeParam.LightMode = %d\r\n",gQRCodeParam.LightMode);
    DBG("gQRCodeParam.ScanMode = %d\r\n",gQRCodeParam.ScanMode);
    DBG("gQRCodeParam.TimeInterval = %d\r\n",gQRCodeParam.TimeInterval);

    gQRCodeParam.SaveQRParam = SaveQRParam;
    gQRCodeParam.SaveQRParam();
    
    return result;
}



