//============================================================================
// �ļ�����: mqx_init.c
// �ļ�����: ����MQX��ʼ��������Ԥ�ȷ���ϵͳռ�ÿռ�
// ��������: 2012-12-13
// ˵����
//     ���ھ����˶�̬�洢����ϵͳ��ȡ����֮����ʹ�þ�̬����洢�ռ�ķ�ʽ
//     ���ַ�ʽ�ʺ���RTOS��Ӧ�ã�ԭ������
//     (1)Ԥ�ȷ���洢�ռ䣬���Լ�������ʱ�������ڴ�ռ䲻�����ڱ���ʱ�Ϳɷ���
//     (2)��̬����洢�ռ䣬Ч�ʵͣ�ͬʱ�����ϵͳ�ĸ��Ӷ�
//============================================================================
#include "mqx_inc.h"

/* MQX stacks */
// ϵͳ�ж�ջ
uint8_t mqx_interrupt_stack[MQX_INTERRUPT_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];
// ϵͳջ
uint8_t    mqx_system_stack[sizeof(TD_STRUCT) + PSP_MINSTACKSIZE + PSP_STACK_ALIGNMENT + 1];
// ��������ջ
uint8_t mqx_idle_task_stack[sizeof(TD_STRUCT) + PSP_IDLE_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];
// �ж�������
INTERRUPT_TABLE_STRUCT    mqx_static_isr_table[MQX_INTERRUPT_TABLE_ITEMS];
// �����������
READY_Q_STRUCT            mqx_static_ready_queue[MQX_READY_QUEUE_ITEMS];

/* Kernel Data */
volatile KERNEL_DATA_STRUCT            mqx_kernel_data;
#define __KERNEL_DATA_START            (pointer)(&mqx_kernel_data)
#define __KERNEL_DATA_END              (pointer)((uchar_ptr)&mqx_kernel_data + sizeof(mqx_kernel_data))

/* MQX Lite initialization structure */
const MQXLITE_INITIALIZATION_STRUCT  MQX_init_struct =
{
/* PROCESSOR NUMBER                 */  1,
/* START OF KERNEL MEMORY           */  __KERNEL_DATA_START,
/* END OF KERNEL MEMORY             */  __KERNEL_DATA_END,
/* MQX_HARDWARE_INTERRUPT_LEVEL_MAX */  1,
/* INTERRUPT STACK SIZE             */  sizeof(mqx_interrupt_stack),
/* INTERRUPT STACK LOCATION         */  mqx_interrupt_stack,
/* IDLE_TASK STACK SIZE             */  sizeof(mqx_idle_task_stack),
/* IDLE_TASK STACK_LOCATION         */  mqx_idle_task_stack,
/* TASK TEMPLATE LIST               */  (TASK_TEMPLATE_STRUCT_PTR)&MQX_template_list[0]
};
