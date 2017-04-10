//===========================================================================
//�ļ����ƣ�02_task_templates.c��mqxlite����ģ���б�
//���ܸ�Ҫ����������ģ���б�ע�����������������ջ
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================
#include "01_app_include.h"

// Ϊ����������ע������ջ
const uint_8 * mqx_task_stack_pointers[] = 
{
	task_main_stack,
	NULL
};
// ����ģ���б�
// ���ڿ����������ȼ����趨���Բ鿴bsp�ļ����µ�mqx_init.h�ļ��е�#define MQX_IDLE_TASK_PRIORITY (16)
// �κ���������ȼ���������ڿ����������ȼ�
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
	// ������,        ������,          ����ջ��С,                    ���ȼ�,  ������,            ��������
	{TASK_MAIN,      task_main,      TASK_MAIN_STACK_SIZE,       7,     "main",           MQX_AUTO_START_TASK},
	{TASK_LIGHT,     task_light,     TASK_LIGHT_STACK_SIZE,      14,    "task_light",     0},
	{TASK_UART0_RE,  task_uart0_re,  TASK_UART0_RECV_STACK_SIZE, 8,     "task_uart0_Re",  0},
	{TASK_RF_RECV,   task_rf_recv,   TASK_RF_RECV_STACK_SIZE,    8,     "task_rf_recv",   0},
	{TASK_RF_SEND,   task_rf_send,   TASK_RF_SEND_STACK_SIZE,    8,     "task_rf_send",   0},
	{TASK_FLASH,     task_flash,     TASK_FLASH_STACK_SIZE,      8,     "task_flash",     0},
	{TASK_REGISTER_PROCESS, task_register_process, TASK_REGISTER_PROCESS_STACK_SIZE, 9 , "task_register_process", 0},
	{0}
};
