#include "cJSON_Process.h"

#define LOG_TAG    "CJSON"
#include "elog.h"

/*******************************************************************
 *                          变量声明                               
 *******************************************************************/



cJSON* cJSON_Data_Init(void)
{
  cJSON* cJSON_Root = NULL;    //json根节点
  
  cJSON_Root = cJSON_CreateObject();   /*创建项目*/
  if(NULL == cJSON_Root)
  {
      return NULL;
  }
  cJSON_AddStringToObject(cJSON_Root, NAME, DEFAULT_NAME);  /*添加元素  键值对*/
  cJSON_AddNumberToObject(cJSON_Root, TEMP_NUM, DEFAULT_TEMP_NUM);
  cJSON_AddNumberToObject(cJSON_Root, HUM_NUM, DEFAULT_HUM_NUM);
  
  char* p = cJSON_Print(cJSON_Root);  /*p 指向的字符串是json格式的*/
  
  log_i("send json = %s\r\n",p); 


  my_free(p);
  p = NULL;
  
  return cJSON_Root;
  
}
uint8_t cJSON_Update(const cJSON * const object,const char * const string,void *d)
{
  cJSON* node = NULL;    //json根节点
  node = cJSON_GetObjectItem(object,string);
  if(node == NULL)
    return NULL;
  if(cJSON_IsBool(node))
  {
    int *b = (int*)d;
//    printf ("d = %d",*b);
    cJSON_GetObjectItem(object,string)->type = *b ? cJSON_True : cJSON_False;
  }
  else if(cJSON_IsString(node))
  {
    cJSON_GetObjectItem(object,string)->valuestring = (char*)d;
  }
  else if(cJSON_IsNumber(node))
  {
    double *num = (double*)d;
    cJSON_GetObjectItem(object,string)->valuedouble = (double)*num;

    my_free(num);
  }

   cJSON_Delete(node);

  return 1;

}

void Proscess(void* data)
{
    log_d("开始解析JSON数据");
    cJSON *root,*json_name,*json_temp_num,*json_hum_num,*json_obj_data;
  
    root = cJSON_Parse((char*)data); //解析成json形式

    if (root == NULL)                 // 如果转化错误，则报错退出
    {
        log_d("1.Error before: [%s]\n", cJSON_GetErrorPtr());
        return ;
    }
      

  json_name = cJSON_GetObjectItem( root , (const char*)"commandCode");  //获取键值内容
  
  json_obj_data = cJSON_GetObjectItem( root , "data" );
  
  json_temp_num = cJSON_GetObjectItem( json_obj_data , "currentModel" );

  json_hum_num = cJSON_GetObjectItem( root , "deviceCode" );

  log_d("code:%s ;currentModel:%d ;deviceCode:%s\r\n",
              json_name->valuestring,
              json_temp_num->valuestring,
              json_hum_num->valuestring);

  cJSON_Delete(root);  //释放内存 

}








