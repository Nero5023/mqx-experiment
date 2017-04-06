//============================================================================
// 文件名称: mqx_init.c
// 文件功能: 定义MQX初始化参数，预先分配系统占用空间
// 更新日期: 2012-12-13
// 说明：
//     由于精简了动态存储管理系统，取而代之的是使用静态分配存储空间的方式
//     这种方式适合于RTOS的应用，原因在于
//     (1)预先分配存储空间，可以减少运行时错误。若内存空间不够，在编译时就可发现
//     (2)动态分配存储空间，效率低，同时提高了系统的复杂度
//============================================================================
#include "mqx_inc.h"

/* MQX stacks */
// 系统中断栈
uint8_t mqx_interrupt_stack[MQX_INTERRUPT_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];
// 系统栈
uint8_t    mqx_system_stack[sizeof(TD_STRUCT) + PSP_MINSTACKSIZE + PSP_STACK_ALIGNMENT + 1];
// 空闲任务栈
uint8_t mqx_idle_task_stack[sizeof(TD_STRUCT) + PSP_IDLE_STACK_SIZE + PSP_STACK_ALIGNMENT + 1];
// 中断向量表
INTERRUPT_TABLE_STRUCT    mqx_static_isr_table[MQX_INTERRUPT_TABLE_ITEMS];
// 就绪任务队列
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
