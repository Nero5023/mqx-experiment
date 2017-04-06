/*
 * bsp.h
 *
 *  Created on: Dec 13, 2012
 *      Author: SY
 */

#ifndef BSP_H_
#define BSP_H_

#include "psptypes.h"
#include "uart.h"
#include "gpio.h"

#define BSP_CORE_CLOCK                  (48000000UL)  // 内核时钟频率(CPU_CORE_CLK_HZ_CONFIG_0)
#define BSP_SYSTEM_CLOCK                (48000000UL)  // 系统时钟频率(CPU_CORE_CLK_HZ_CONFIG_0)

// SYSTICK的频率
#define BSP_ALARM_FREQUENCY             (200)
// SYSTICK的周期（ms）5ms/tick
#define BSP_ALARM_RESOLUTION            (1000 / BSP_ALARM_FREQUENCY)

// 定义系统滴答定时器中断
#define BSP_TIMER_INTERRUPT_VECTOR              INT_SysTick   // 来自于芯片头文件
#define BSP_TIMER_INTERRUPT_PRIORITY            0    // CM0中仅有两个可编程优先级级别

typedef void (*const tIsrFunc)(void);
//typedef struct 
//{
//	void * __ptr;
//	tIsrFunc __fun[0x2F];
//} tVectorTable;

extern uint_32 _bsp_enable_card ( void );

#endif /* BSP_H_ */
