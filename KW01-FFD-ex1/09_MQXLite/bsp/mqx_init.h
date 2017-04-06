/*
 * mqxlite_prv_inc.h
 *
 *  Created on: Dec 13, 2012
 *      Author: SY
 */

#ifndef MQX_INIT_H_
#define MQX_INIT_H_

// MQX系统栈
extern uint8_t mqx_system_stack[];

/* MQX Lite template list and init structure */
extern const TASK_TEMPLATE_STRUCT      MQX_template_list[];
extern const MQXLITE_INITIALIZATION_STRUCT MQX_init_struct;

/* The first interrupt allocated by Processor Expert */
#define FIRST_INTERRUPT_VECTOR_USED    (INT_SysTick)
/* The last interrupt allocated by Processor Expert */
#define LAST_INTERRUPT_VECTOR_USED     (INT_PORTD)//(INT_LVD_LVW)

#define MQX_INTERRUPT_TABLE_ITEMS      (LAST_INTERRUPT_VECTOR_USED - FIRST_INTERRUPT_VECTOR_USED + 1)
extern INTERRUPT_TABLE_STRUCT          mqx_static_isr_table[MQX_INTERRUPT_TABLE_ITEMS];

// 系统中的最低任务优先级
#define MQX_IDLE_TASK_PRIORITY         (16)
#define MQX_READY_QUEUE_ITEMS          (MQX_IDLE_TASK_PRIORITY + 1)
extern READY_Q_STRUCT                  mqx_static_ready_queue[MQX_READY_QUEUE_ITEMS];

extern const uint8_t * mqx_task_stack_pointers[];

#endif /* MQXLITE_PRV_INC_H_ */
