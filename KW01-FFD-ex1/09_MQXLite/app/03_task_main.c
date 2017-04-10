//===========================================================================
//�ļ����ƣ�03_task_main.c������������
//���ܸ�Ҫ������MQXLite������������
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================
#include "01_app_include.h"

//===========================================================================
//�������ƣ�task_main
//���ܸ�Ҫ��MQX������������Ҫʵ�����������ʼ���������������񡢰�װ�û�ISR
//����˵����δʹ��
//===========================================================================
void task_main(uint32_t initial_data)
{
	//1. ����������ʹ�õı���
	//1�������������¼���
	_lwevent_create(&lwevent_group,0);

	HD_adr=0xFF;

	//2. �����ж�
    DISABLE_INTERRUPTS;

    //3. ��ʼ������ģ��
    uart_init(UART_0,MCGFLLCLK,9600);    //��ʼ��UART_0ģ��
    light_init(LIGHT_RUN_1,LIGHT_OFF);   //��ʼ���ڵ������ָʾ��1
    flash_init();                        //��ʼ��Flash
    RF_Init(HD_adr);		         //RFģ���ʼ��

    //4. ���йر�������ֵ
    seconds=0;

	//��ʼ����Ϣ����
	_lwmsgq_init((pointer)pccommand_queue,COMMAND_NUM_MESSAGES,COMMAND_MSG_SIZE);
	_lwmsgq_init((pointer)register_queue,RE_NUM_MESSAGES,RE_MSG_SIZE);


    //5. ������������
    _task_create_at(0, TASK_LIGHT,   0, task_light_stack,      TASK_LIGHT_STACK_SIZE);
    _task_create_at(0, TASK_RF_RECV, 0, task_rf_recv_stack,    TASK_RF_RECV_STACK_SIZE);
    _task_create_at(0, TASK_UART0_RE,0, task_uart0_Recv_stack, TASK_UART0_RECV_STACK_SIZE);
    _task_create_at(0, TASK_RF_SEND, 0, task_rf_send_stack,    TASK_RF_SEND_STACK_SIZE);
    _task_create_at(0, TASK_FLASH,   0, task_flash_stack,       TASK_FLASH_STACK_SIZE);
    _task_create_at(0, TASK_REGISTER_PROCESS,0,task_register_process_stack, TASK_REGISTER_PROCESS_STACK_SIZE);
    _task_create_at(0, TASK_PC_COMMAND,0,task_pc_command_stack, TASK_PC_COMMAND_STACK_SIZE);

    //6. ��װ�û�ISR
    _int_install_isr(INT_UART0,isr_uart0_re,NULL);//����0��ISR
    _int_install_isr(INT_PORTD,gpio_CD_ISR,NULL);     //PTC��PTD��ISR���˿�C��D����ͬһ�ж�����

    //7. ʹ��ģ���ж�
	uart_enable_re_int(UART_0);   //ʹ�ܴ���0�����ж�
	MKW01Drv_IRQ_DIO1_Enable();	  //����DIO1�ж�



	//8. �����ж�
	ENABLE_INTERRUPTS;            //�����ж�
    //------------ִ����ϣ��������������̬----------------------------------------------

    _task_block();
}
