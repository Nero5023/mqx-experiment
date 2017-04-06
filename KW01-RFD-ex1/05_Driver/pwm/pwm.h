//===========================================================================
//文件名称：pwm.c
//功能概要：pwm底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版    本：V3.0(WYH),20140909
//备    注：SD-FSL-KW01节点中有TPM0、TPM1和TPM2共3个TPM模块，每个模块又有
//          若干通道，都可以配置产生边沿对齐或是中心对齐的PWM信号，
//          每个通道的具体引脚分配如下：
//             通道                           引脚名(复用编号)
//             TPM0_CH0      PTC1(4)
//             TPM0_CH1      PTC2(4)、PTA4(3)
//             TPM0_CH5      PTD5(4)
//        
//             TPM1_CH0      PTB0(3)    
//             TPM1_CH1      PTB1(3)
//
//             TPM2_CH0      PTB2(3)、PTA1(3)
//             TPM2_CH1      PTA2(3)
//初始化默认使用通道：PTC1、PTC2、PTD5、PTB0、PTB1、PTB2、PTA2
//PTA1、PTA2如果作为串口0收发引脚使用，不建议使用这两个通道
//===========================================================================
#ifndef _PWM_H
#define _PWM_H

#include "common.h"

//tpm模块irq号
#define TPM0_IRQ_NO 17
#define TPM1_IRQ_NO 18
#define TPM2_IRQ_NO 19
//tpm模块号
#define TPM0  0
#define TPM1  1
#define TPM2  2
//tpm通道号
#define TPMCH0  0
#define TPMCH1  1
#define TPMCH2  2
#define TPMCH3  3
#define TPMCH4  4
#define TPMCH5  5

//PWM初始化过程中，TPM0_CH1_PIN宏定义为1，TPM2_CH0_PIN宏定义为1。
//TPM0_CH1：1=PTC2(4)，2=PTA4(3)
#define TPM0_CH1_PIN 1

//TPM2_CH0：1=PTB2(3)，2=PTA1(3)
#define TPM2_CH0_PIN 1

//===========================================================================
//函数名称：pwm_init                                                        
//功能概要：pwm模块初始化。                                                
//参数说明：tpmModule：模块号：TPM0、TPM1、TPM2
//          channel：通道号:TPMCH0-TPMCH5
//          period：脉冲周期，单位微秒（例如1000为1ms）
//          duty：占空比，取值：1-100
//函数返回：无
//===========================================================================
void pwm_init(uint_8 tpmModule,uint_8 channel,uint_32 period,uint_8 duty);

//=========================================================================
//函数名称：pwm_stop
//功能概要：关闭PWM模块
//参数说明：tpmModule(模块号)：TPM0,TPM1,TPM2
//函数返回：无
//=========================================================================
void pwm_stop(uint_8 tpmModule);

//=========================================================================
//函数名称：pwm_enable_int
//功能概要：使能PWM中断
//参数说明：tpmModule(模块号)：TPM0,TPM1,TPM2
//函数返回：无
//=========================================================================
void pwm_enable_int(uint_8 tpmModule);

//=========================================================================
//函数名称：pwm_disable_int
//功能概要：禁止PWM中断
//参数说明：tpmModule(模块号)：TPM0,TPM1,TPM2
//函数返回：无
//=========================================================================
void pwm_disable_int(uint_8 tpmModule);

#endif 
