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
#include "Protocol.h"
#include "cp.h"
#include "address_manage.h"

#ifndef GLOBAL_VAR
#define GLOBAL_VAR_PRE extern
#else
#define GLOBAL_VAR_PRE
#endif

#define SELF_ADDR 0x09

// Lage_Data_Flag �����ݷ��ͱ�־λ
#define CAN_NOT_SEND 0
#define CAN_SEND 1
#define IS_SENDING 2


//1.2 �궨�峣��

//1.3 ����ȫ�ֱ���
uint_8 g_uart_recvBuf[72];    //���ڽ�������ȫ������
uint_8 g_uart_sentBuf[72];    //���ڷ�������ȫ������

uint_8 rf_recvBuf[64];          //��Ž������ݰ�
uint_8 rf_sentBuf[64];          //RF�������ݳ���+�������ݣ��������ݳ��Ȳ�����64�ֽڣ�

uint_8 HD_adr;
uint_8 rf_sentDataLength;

GLOBAL_VAR_PRE char rfdAddrs[8];        // store the rfd addrs
GLOBAL_VAR_PRE char rfdKeys[9];         // store the rfdkeys

uint_8 ADDR_INFO[10][3];  // store register info 

uint_8 Lage_Data_Flag;   // 0: ���ܷ��ͣ�1: ���Է��ͣ�2�����ڷ���


MUTEX_STRUCT mutex_rf_recvBuf;  //���ղ��������ź���
uint32_t seconds;	          //PIT��¼��λ���ʱ�䣨��λ���룩
float g_temperature;          //оƬ�¶�
uint_8 g_rfRecCount;


//1.4 �����������¼���
LWEVENT_STRUCT  lwevent_group;

//1.5 �궨���¼�λ
#define  EVENT_RF_RECV        ((1uL)<<(1))   //RF�����¼�λ
#define  EVENT_RF_SEND        ((1uL)<<(2))   //RF�����¼�λ
#define  EVENT_UART0_RE       ((1uL)<<(4))   //����0������������֡�¼�λ
#define  EVENT_RECEIVE_CTS    ((1uL)<<(6))   //�յ� CTS ����

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
#define TASK_REGISTER_PROCESS 10
#define TASK_PC_COMMAND    11

//2.2 �궨������ջ��С
#define TASK_MAIN_STACK_SIZE       (sizeof(TD_STRUCT) + 400 + PSP_STACK_ALIGNMENT + 1)
#define TASK_LIGHT_STACK_SIZE      (sizeof(TD_STRUCT) + 200 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_RECV_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_UART0_RECV_STACK_SIZE (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_RF_SEND_STACK_SIZE    (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)
#define TASK_REGISTER_PROCESS_STACK_SIZE    (sizeof(TD_STRUCT) + 400 + PSP_STACK_ALIGNMENT + 1)
#define TASK_PC_COMMAND_STACK_SIZE  (sizeof(TD_STRUCT) + 400 + PSP_STACK_ALIGNMENT + 1)

//2.3 ����������
void task_main(uint32_t initial_data);
void task_light(uint32_t initial_data);
void task_rf_recv(uint32_t initial_data);
void task_uart0_re(uint32_t initial_data);
void task_rf_send(uint32_t initial_data);
void task_register_process(uint32_t initial_data);
void task_pc_command(uint32_t initial_data);

//2.4 ��������ջ
uint_8 task_main_stack[TASK_MAIN_STACK_SIZE];
uint_8 task_light_stack[TASK_LIGHT_STACK_SIZE];
uint_8 task_rf_recv_stack[TASK_RF_RECV_STACK_SIZE];
uint_8 task_rf_send_stack[TASK_RF_SEND_STACK_SIZE];
uint_8 task_uart0_Recv_stack[TASK_UART0_RECV_STACK_SIZE];
uint_8 task_register_process_stack[TASK_REGISTER_PROCESS_STACK_SIZE];
uint_8 task_pc_command_stack[TASK_PC_COMMAND_STACK_SIZE];

//3.1��Ϣ����
#define COMMAND_NUM_MESSAGES  3
#define COMMAND_MSG_SIZE      16
uint_32 pccommand_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint_32)+COMMAND_NUM_MESSAGES*COMMAND_MSG_SIZE]; //pcָ����Ϣ����


#define REGISTER_NUM_MESSAGES  3
#define REGISTER_MSG_SIZE      16
uint_32 register_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint_32)+REGISTER_NUM_MESSAGES*REGISTER_MSG_SIZE]; //�ӻ�ע����Ϣ����


#define RECV_NUM_MESSAGES  3
#define RECV_MSG_SIZE      16
uint_32 recv_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint_32)+RECV_NUM_MESSAGES*RECV_MSG_SIZE]; //����������ָ����Ϣ����

#define SEND_NUM_MESSAGES  3
#define SEND_MSG_SIZE      16
uint_32 send_queue[sizeof(LWMSGQ_STRUCT)/sizeof(uint_32)+SEND_NUM_MESSAGES*SEND_MSG_SIZE]; //�����͵�ָ����Ϣ����










//---------------------------------------------------------------------------
#endif    //01_app_include.h
