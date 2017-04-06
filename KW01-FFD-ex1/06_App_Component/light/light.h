//============================================================================
//文件名称：light.h
//功能概要：小灯构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-10-09   V1.0
//         2013-02-02   V2.0(WYH)
//============================================================================

#ifndef Light_H     //防止重复定义（Light_H  开头)
#define Light_H

//头文件包含
#include "common.h"    //包含公共要素头文件
#include "gpio.h"      //用到gpio构件

//指示灯端口及引脚定义
#define LIGHT_RUN_1         (PORTD|(7u))   //指示灯1(节点)使用的端口与引脚
#define LIGHT_RUN_2         (PORTB|(0u))   //指示灯2(底座)使用的端口与引脚

//灯状态宏定义（灯亮、灯暗对应的物理电平由硬件接法决定）
#define LIGHT_ON        0    //灯亮
#define LIGHT_OFF       1    //灯暗

//=================接口函数声明===============================================
//============================================================================
//函数名称：light_init
//函数参数：port_pin：端口及引脚名。由light.h中宏定义。
//         state：设定小灯状态。由light.h中宏定义。
//函数返回：无
//功能概要：指示灯驱动初始化。
//============================================================================
void light_init(uint_16 port_pin, uint_8 state);

//============================================================================
//函数名称：light_control
//函数参数：port_pin：端口及引脚名。由light.h中宏定义。
//         state：设定小灯状态。由light.h中宏定义。
//函数返回：无
//功能概要：控制指示灯亮暗。
//============================================================================
void light_control(uint_16 port_pin, uint_8 state);

//============================================================================
//函数名称：light_change
//函数参数：port_pin：端口及引脚名。由light.h中宏定义。
//函数返回：无
//功能概要：切换指示灯亮暗。
//============================================================================
void light_change(uint_16 port_pin);

#endif    //防止重复定义（Light_H  结尾)
