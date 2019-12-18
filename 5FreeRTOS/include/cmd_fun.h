/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : cmd_fun.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月31日
  最近修改   :
  功能描述   : CLI 指令的初始化及实现
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月31日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __CMD_FUN_H
#define __CMD_FUN_H

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"



/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#ifndef  configINCLUDE_TRACE_RELATED_CLI_COMMANDS
	#define configINCLUDE_TRACE_RELATED_CLI_COMMANDS 0
#endif
    
#ifndef configINCLUDE_QUERY_HEAP_COMMAND
	#define configINCLUDE_QUERY_HEAP_COMMAND 0
#endif


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

extern void CMD_Init(void);

/*
 * The function that registers the commands that are defined within this file.
 */
//extern void vRegisterSampleCLICommands( void );

/*
 * Implements the task-stats command.
 */
static BaseType_t prvTaskStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the run-time-stats command.
 */
#if( configGENERATE_RUN_TIME_STATS == 1 )
    static BaseType_t prvRunTimeStatsCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif /* configGENERATE_RUN_TIME_STATS */

/*
 * Implements the echo-three-parameters command.
 */
static BaseType_t prvThreeParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the echo-parameters command.
 */
static BaseType_t prvParameterEchoCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/*
 * Implements the "query heap" command.
 */
#if( configINCLUDE_QUERY_HEAP_COMMAND == 1 )
    static BaseType_t prvQueryHeapCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

/*
 * Implements the "trace start" and "trace stop" commands;
 */
#if( configINCLUDE_TRACE_RELATED_CLI_COMMANDS == 1 )
    static BaseType_t prvStartStopTraceCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
#endif

//打印参数列表
static BaseType_t prvPrintEnv(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//设置参数
static BaseType_t prvSetEnv(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//由key值获取值
static BaseType_t prvGetEnv(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//删除key的值
static BaseType_t prvDelEnv(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//初始化参数区
static BaseType_t prvResetEnv(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);









#endif

