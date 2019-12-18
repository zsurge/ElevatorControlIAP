/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : tool.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月20日
  最近修改   :
  功能描述   : 工具类
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月20日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "tool.h"
#include "stdarg.h"
#include "stdio.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
// CRC 高位字节值表
const uint8_t CrcHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
// CRC 低位字节值表
const uint8_t CrcLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
 
/*************************************************************************************
*   函 数 名: CRC16_Modbus
*   功能说明: 计算CRC。 用于Modbus协议。
*   形    参: pBuf : 参与校验的数据
*             usLen : 数据长度
*   返 回 值: 16位整数值。 对于Modbus ，此结果高字节先传送，低字节后传送。
*
*   所有可能的CRC值都被预装在两个数组当中，当计算报文内容时可以简单的索引即可；
*   一个数组包含有16位CRC域的所有256个可能的高位字节，另一个数组含有低位字节的值；
*   这种索引访问CRC的方式提供了比对报文缓冲区的每一个新字符都计算新的CRC更快的方法；
*
*  注意：此程序内部执行高/低CRC字节的交换。此函数返回的是已经经过交换的CRC值；也就是说，
*        该函数的返回值可以直接放置
*        于报文用于发送；
************************************************************************************/
uint16_t CRC16_Modbus(uint8_t *pBuf, uint16_t usLen)
{
    uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
    uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
    uint16_t  usIndex;  /* CRC循环中的索引 */

    while (usLen--)
    {
        usIndex = ucCRCLo ^ *pBuf++; /* 计算CRC */
        ucCRCLo = ucCRCHi ^ CrcHi[usIndex];
        ucCRCHi = CrcLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}


/*****************************************************************************
 函 数 名  : asc2bcd
 功能描述  :     ascii码转bcd码 "123456"=====>0x12,0x34,0x56
 输入参数  :     unsigned char *bcd_buf     存放BCD码的位置   
             unsigned char *ascii_buf   要转换ascii字符串的位置
             int conv_len               ascii字符串的长度
             unsigned char type         0:后补0,"12345"==>0x12,0x34,0x50
                                        1:前补0,"12345"==>0x01,0x23,0x45
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2019年7月5日
    作    者   : 张舵
void asc2bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, unsigned char type)
    修改内容   : 新生成函数

*****************************************************************************/
void asc2bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, unsigned char type)
{
    int    cnt;
    char   ch, ch1;

    if (conv_len&0x01 && type ) ch1=0;
    else ch1=0x55;
    for (cnt=0; cnt<conv_len; ascii_buf++, cnt++)
    {
        if (*ascii_buf >= 'a' ) ch = *ascii_buf-'a' + 10;
        else if ( *ascii_buf >= 'A' ) ch =*ascii_buf- 'A' + 10;
        else if ( *ascii_buf >= '0' ) ch =*ascii_buf-'0';
        else ch = 0;
        if (ch1==0x55) ch1=ch;
        else {
            *bcd_buf++=( ch1<<4 )| ch;
            ch1=0x55;
        }
    }
    if (ch1!=0x55) *bcd_buf=ch1<<4;
}


/*****************************************************************************
 函 数 名  : bcd2asc
 功能描述  : hex 转 ascii 码  \x12\x34\x56 ===> "123456"
 输入参数  :   unsigned char *ascii_buf  存放ascii的值位置
           unsigned char * bcd_buf   要转换的hex字符串
           int conv_len              要转换的hex字符串*2
           unsigned char type        0:后删1，0x12,0x34,0x56(长度=5) ===>"12345"
                                     1:前删1，0x12,0x34,0x56(长度=5) ===>"23456"
 输出参数  : 无
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2019年7月5日
    作    者   : 张舵
void bcd2asc(unsigned char *ascii_buf,unsigned char * bcd_buf, int conv_len, unsigned char type)
    修改内容   : 新生成函数

*****************************************************************************/
void bcd2asc(unsigned char *ascii_buf,unsigned char * bcd_buf, int conv_len, unsigned char type)
{
    int cnt=0;

    if (conv_len&0x01 && type) {cnt=1; conv_len++;}
    else cnt=0;
    for (; cnt<conv_len; cnt++, ascii_buf++){
        *ascii_buf = ((cnt&0x01) ? (*bcd_buf++&0x0f) : (*bcd_buf>>4));
        *ascii_buf += ((*ascii_buf>9) ? ('A'-10) : '0');
    }
}


/*****************************************************************************
 函 数 名  : xorCRC
 功能描述  : 计算异或校验值
 输入参数  : uint8_t *buf  要计算的缓冲区头
           uint8_t len 要计算的长度  
 输出参数  : 无
 返 回 值  : CRC值
 
 修改历史      :
  1.日    期   : 2019年7月5日
    作    者   : 张舵
uint8_t xorCRC(uint8_t *buf,uint8_t len)
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t xorCRC(uint8_t *buf,uint8_t len)
{
    uint8_t i = 0;
    uint8_t bcc = 0;
    bcc = buf[0];

    for(i=1;i<len;i++)
    {
        bcc ^= buf[i];
    }

    return bcc;
}
    
/*****************************************************************************
 函 数 名  : is_bit_set
 功能描述  : 判定value的第bit位是否为1
 输入参数  : uint16_t      value  
             uint16_t bit         
 输出参数  : 无
 返 回 值  : 1 该bit为1
             0 该bit为0
 
 修改历史      :
  1.日    期   : 2019年7月23日
    作    者   : 张舵
uint8_t is_bit_set(uint16_t      value, uint16_t bit)
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t is_bit_set(uint16_t      value, uint16_t bit)
{
    if (((value >> bit) & 1) == 1)
    {
        return 1;
    }

    return 0; 
}

void dbg(const char *file, const long line, const char *format, ...)
{
    #ifdef DEBUG_PRINT
    va_list args;
	char debug_buf[1280] = {0};	
    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[DEBUG](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(debug_buf, sizeof(debug_buf), format, args);
    printf("%s\n", debug_buf);
    va_end(args);
    #endif

}

void dbh(char *title,char *buf,int len)
{
    #ifdef DEBUG_PRINT
    int i = 0;
    printf("<%s,len=%d>:<",title,len);
    for(i=0;i<len;i++)
    {
        printf("%02x ",buf[i]);
    }
    printf(" >\r\n");   
    #endif
}


/*****************************************************************************
 函 数 名  : Int2Str
 功能描述  : 整数转换为字符串  。int a = 3008 ===>  "3008"
 输入参数  : int32_t intnum                
 输出参数  : uint8_t* str
 返 回 值  : 
 
 修改历史      :
  1.日    期   : 2019年7月5日
    作    者   : 张舵
    void Int2Str(uint8_t* str, int32_t intnum)
    修改内容   : 新生成函数

*****************************************************************************/
void int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 48;

		intnum = intnum % Div;
		Div /= 10;
		if ((str[j - 1] == '0') & (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
}

/*****************************************************************************
 函 数 名  : str2int
 功能描述  : ascii码格式的hexstring转为整型 "1234" ===> 1234
 输入参数  : const char* str  
 输出参数  : 无
 返 回 值  : 返回字符串的整型值
 
 修改历史      :
  1.日    期   : 2019年7月5日
    作    者   : 张舵
    int32_t str2int(const char* str)
    修改内容   : 新生成函数

*****************************************************************************/
int32_t str2int(const char* str)
{
	int32_t temp = 0;
	const char* ptr = str;			//ptr保存str字符串开头  
	if (*str == '-' || *str == '+')	//如果第一个字符是正负号，
	{								//则移到下一个字符
		str++;
	}
	while (*str != 0)
	{
		if ((*str < '0') || (*str > '9'))  //如果当前字符不是数字
		{					                //则退出循环
			break;
		}
		temp = temp * 10 + (*str - '0');    //如果当前字符是数字则计算数值
		str++;                              //移到下一个字符
	}
	if (*ptr == '-')                        //如果字符串是以“-”开头，则转换成其相反数
	{
		temp = -temp;
	}

	return temp;
}
 
char *strstr_t(const char *str, const char *sub_str)
{   
    const char *str_local = NULL;
    const char *sub_str_local = NULL;
 
    if(!str || !sub_str)
    {   
        printf("fun:%s param is error\n", __FUNCTION__);
        return NULL;
    }
 
    while(*str)
    {
        str_local = str;
        sub_str_local = sub_str;
 
        do
        {
            if(*sub_str_local == '\0')
            {   
                return (char *)str;
            }
        }while(*str_local++ == *sub_str_local++);
        str += 1;
    }
    
    return NULL;
}

 



