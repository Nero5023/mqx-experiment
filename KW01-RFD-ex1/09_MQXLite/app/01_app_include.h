//===========================================================================
//文件名称：01_app_include.h
//功能概要：应用任务公共头文件（MQX中头文件及相关全局变量、任务参数的声明等）
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================

#ifndef __01_APP_INCLUDE_H_
#define __01_APP_INCLUDE_H_

//---------------------------------------------------------------------------
// 第一部分
// 包含头文件、宏定义常量、声明全局变量、声明轻量级事件组、定义事件位
//---------------------------------------------------------------------------

//1.1 包含头文件
#include "bsp.h"
#include "stdint.h"
#include "mqxlite.h"
#include "mqx_inc.h"
#include "light.h"
#include "lwevent.h"
#include "rf.h"
#include "lwmsgq.h"
#include "mutex.h"
#include "flash.h"
#include "wp.h"

//1.2 宏定义常量

//1.3 声明全局变量
uint_8 g_uart_recvBuf[72];    //串口接收数据全局数组
uint_8 g_uart_sentBuf[72];    //串口发送数据全局数组

uint_8 rf_recvBuf[64];          //存放接收数据包
uint_8 rf_sentBuf[64];          //RF发送数据长度+数据内容（数据内容长度不超过64字节）

uint_8 HD_adr;


MUTEX_STRUCT mutex_rf_recvBuf;  //接收操作互斥信号量
uint32_t seconds;	          //PIT记录复位后的时间（单位：秒）
float g_temperature;          //芯片温度
uint_8 g_rfRecCount;
uint_8 rf_sentDataLength;


//1.4 声明轻量级事件组
LWEVENT_STRUCT  lwevent_group;

//1.5 宏定义事件位
#define  EVENT_RF_RECV        ((1uL)<<(1))   //RF接收事件位
#define  EVENT_RF_SEND        ((1uL)<<(2))   //RF发送事件位
#define  EVENT_UART0_RE       ((1uL)<<(4))   //串口0接收完整数据帧事件位
#define  EVENT_FLASH          ((1uL)<<(5))   //Flash操作事件位

//---------------------------------------------------------------------------
// 第二部分
// 新建任务的相关定义
//---------------------------------------------------------------------------
// 添加任务的步骤
// (1) 在02_task_templates.c中任务模板列表中添加任务模板 （任务编号,任务函数,任务栈大小,优先级,任务名,任务属性）
// (2) 在01_app_include.h中宏定义任务编号、宏定义任务栈大小、声明任务函数、声明任务栈
// (3) 在03_task_main.c的自启动任务中，创建任务_task_create
//---------------------------------------------------------------------------

//2.1 宏定义任务编号
#define TASK_MAIN          1
#define TASK_LIGHT         2
#define TASK_RF_RECV       3
#define TASK_RF_SEND       4
#define TASK_UART          5
#define TASK_UART0_RE      8
#define TASK_FLASH	       9
#define TASK_WP_REGISTER   10

//2.2 宏定义任务栈大小
#define TASK_MAIN_STACK_SIZE       (sizeof(TD_STRUCT) + 400 + PSP_STACK_ALIGNMENT + 1)
#define TASK_LIGHT_STACK_SIZE      (sizeof(TD_STRUCT) + 200 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_RECV_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_UART0_RECV_STACK_SIZE (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_SEND_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_FLASH_STACK_SIZE      (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)///TODO:大小是否合适？
#define TASK_WP_REGISTER_STACK_SIZE    (sizeof(TD_STRUCT) + 200 + PSP_STACK_ALIGNMENT + 1)

//2.3 声明任务函数
void task_main(uint32_t initial_data);
void task_light(uint32_t initial_data);
void task_rf_recv(uint32_t initial_data);
void task_uart0_re(uint32_t initial_data);
void task_rf_send(uint32_t initial_data);
void task_flash(uint32_t initial_data);
void task_wp_register(uint32_t initial);


//2.4 声明任务栈
uint_8 task_main_stack[TASK_MAIN_STACK_SIZE];
uint_8 task_light_stack[TASK_LIGHT_STACK_SIZE];
uint_8 task_rf_recv_stack[TASK_RF_RECV_STACK_SIZE];
uint_8 task_rf_send_stack[TASK_RF_SEND_STACK_SIZE];
uint_8 task_uart0_Recv_stack[TASK_UART0_RECV_STACK_SIZE];
uint_8 task_flash_stack[TASK_FLASH_STACK_SIZE];
uint_8 task_wp_register_stack[TASK_WP_REGISTER_STACK_SIZE];

//---------------------------------------------------------------------------
#endif    //01_app_include.h
