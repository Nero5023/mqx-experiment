//===========================================================================
//文件名称：02_task_templates.c（mqxlite任务模板列表）
//功能概要：放置任务模板列表及注册自启动任务的任务栈
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"

// 为自启动任务注册任务栈
const uint_8 * mqx_task_stack_pointers[] = 
{
	task_main_stack,
	NULL
};
// 任务模板列表
// 对于空闲任务优先级的设定可以查看bsp文件夹下的mqx_init.h文件中的#define MQX_IDLE_TASK_PRIORITY (16)
// 任何任务的优先级都必须高于空闲任务优先级
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
	// 任务编号,        任务函数,          任务栈大小,                    优先级,  任务名,            任务属性
	{TASK_MAIN,      task_main,      TASK_MAIN_STACK_SIZE,       7,     "main",           MQX_AUTO_START_TASK},
	{TASK_LIGHT,     task_light,     TASK_LIGHT_STACK_SIZE,      14,    "task_light",     0},
	{TASK_UART0_RE,  task_uart0_re,  TASK_UART0_RECV_STACK_SIZE, 8,     "task_uart0_Re",  0},
	{TASK_RF_RECV,   task_rf_recv,   TASK_RF_RECV_STACK_SIZE,    8,     "task_rf_recv",   0},
	{TASK_RF_SEND,   task_rf_send,   TASK_RF_SEND_STACK_SIZE,    8,     "task_rf_send",   0},
	{TASK_FLASH,     task_flash,     TASK_FLASH_STACK_SIZE,      8,     "task_flash",     0},
	{TASK_REGISTER_PROCESS, task_register_process, TASK_REGISTER_PROCESS_STACK_SIZE, 9 , "task_register_process", 0},
	{0}
};
