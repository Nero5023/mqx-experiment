//============================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2012-11-12  V2.0
//============================================================================

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include "mqxlite.h"
#include "io.h"
#include "common.h"
#include "uart.h"
#include "sysinit.h"
#include "rf.h"
#include "isr.h"

//定义全局变量

//定义系统时钟、总线时钟
#define  CORE_CLK           48000   //芯片内核时钟频率，kHz
#define  BUS_CLK            24000   //芯片总线时钟频率，Khz
#define  MCGFLL_CLK         24000   //芯片内核时钟频率，kHz
#define  MCGIR_CLK          4000    //芯片内核时钟频率，kHz

//定义小灯闪烁频率
#define RUN_COUNTER_MAX   1500000ul 

//定义使用的调试号
#define UART_TEST UART_0




#endif
