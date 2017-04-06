//============================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2012-11-12  V2.0
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

//����ȫ�ֱ���

//����ϵͳʱ�ӡ�����ʱ��
#define  CORE_CLK           48000   //оƬ�ں�ʱ��Ƶ�ʣ�kHz
#define  BUS_CLK            24000   //оƬ����ʱ��Ƶ�ʣ�Khz
#define  MCGFLL_CLK         24000   //оƬ�ں�ʱ��Ƶ�ʣ�kHz
#define  MCGIR_CLK          4000    //оƬ�ں�ʱ��Ƶ�ʣ�kHz

//����С����˸Ƶ��
#define RUN_COUNTER_MAX   1500000ul 

//����ʹ�õĵ��Ժ�
#define UART_TEST UART_0




#endif
