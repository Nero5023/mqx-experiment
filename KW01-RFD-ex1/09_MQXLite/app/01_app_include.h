//===========================================================================
//�ļ����ƣ�01_app_include.h
//���ܸ�Ҫ��Ӧ�����񹫹�ͷ�ļ���MQX��ͷ�ļ������ȫ�ֱ�������������������ȣ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================

#ifndef __01_APP_INCLUDE_H_
#define __01_APP_INCLUDE_H_

//---------------------------------------------------------------------------
// ��һ����
// ����ͷ�ļ����궨�峣��������ȫ�ֱ����������������¼��顢�����¼�λ
//---------------------------------------------------------------------------

//1.1 ����ͷ�ļ�
#include "bsp.h"
#include "stdint.h"
#include "mqxlite.h"
#include "mqx_inc.h"
#include "light.h"
#include "lwevent.h"
#include "rf.h"
#include "lwmsgq.h"
#include "mutex.h"
#include "flash.h"
#include "wp.h"

//1.2 �궨�峣��

//1.3 ����ȫ�ֱ���
uint_8 g_uart_recvBuf[72];    //���ڽ�������ȫ������
uint_8 g_uart_sentBuf[72];    //���ڷ�������ȫ������

uint_8 rf_recvBuf[64];          //��Ž������ݰ�
uint_8 rf_sentBuf[64];          //RF�������ݳ���+�������ݣ��������ݳ��Ȳ�����64�ֽڣ�

uint_8 HD_adr;


MUTEX_STRUCT mutex_rf_recvBuf;  //���ղ��������ź���
uint32_t seconds;	          //PIT��¼��λ���ʱ�䣨��λ���룩
float g_temperature;          //оƬ�¶�
uint_8 g_rfRecCount;
uint_8 rf_sentDataLength;


//1.4 �����������¼���
LWEVENT_STRUCT  lwevent_group;

//1.5 �궨���¼�λ
#define  EVENT_RF_RECV        ((1uL)<<(1))   //RF�����¼�λ
#define  EVENT_RF_SEND        ((1uL)<<(2))   //RF�����¼�λ
#define  EVENT_UART0_RE       ((1uL)<<(4))   //����0������������֡�¼�λ
#define  EVENT_FLASH          ((1uL)<<(5))   //Flash�����¼�λ

//---------------------------------------------------------------------------
// �ڶ�����
// �½��������ض���
//---------------------------------------------------------------------------
// �������Ĳ���
// (1) ��02_task_templates.c������ģ���б����������ģ�� ��������,������,����ջ��С,���ȼ�,������,�������ԣ�
// (2) ��01_app_include.h�к궨�������š��궨������ջ��С����������������������ջ
// (3) ��03_task_main.c�������������У���������_task_create
//---------------------------------------------------------------------------

//2.1 �궨��������
#define TASK_MAIN          1
#define TASK_LIGHT         2
#define TASK_RF_RECV       3
#define TASK_RF_SEND       4
#define TASK_UART          5
#define TASK_UART0_RE      8
#define TASK_FLASH	       9
#define TASK_WP_REGISTER   10

//2.2 �궨������ջ��С
#define TASK_MAIN_STACK_SIZE       (sizeof(TD_STRUCT) + 400 + PSP_STACK_ALIGNMENT + 1)
#define TASK_LIGHT_STACK_SIZE      (sizeof(TD_STRUCT) + 200 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_RECV_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_UART0_RECV_STACK_SIZE (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_SEND_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_FLASH_STACK_SIZE      (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)///TODO:��С�Ƿ���ʣ�
#define TASK_WP_REGISTER_STACK_SIZE    (sizeof(TD_STRUCT) + 200 + PSP_STACK_ALIGNMENT + 1)

//2.3 ����������
void task_main(uint32_t initial_data);
void task_light(uint32_t initial_data);
void task_rf_recv(uint32_t initial_data);
void task_uart0_re(uint32_t initial_data);
void task_rf_send(uint32_t initial_data);
void task_flash(uint32_t initial_data);
void task_wp_register(uint32_t initial);


//2.4 ��������ջ
uint_8 task_main_stack[TASK_MAIN_STACK_SIZE];
uint_8 task_light_stack[TASK_LIGHT_STACK_SIZE];
uint_8 task_rf_recv_stack[TASK_RF_RECV_STACK_SIZE];
uint_8 task_rf_send_stack[TASK_RF_SEND_STACK_SIZE];
uint_8 task_uart0_Recv_stack[TASK_UART0_RECV_STACK_SIZE];
uint_8 task_flash_stack[TASK_FLASH_STACK_SIZE];
uint_8 task_wp_register_stack[TASK_WP_REGISTER_STACK_SIZE];

//---------------------------------------------------------------------------
#endif    //01_app_include.h
