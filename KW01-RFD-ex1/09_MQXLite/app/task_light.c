//===========================================================================
//�ļ����ƣ�task_light.c
//���ܸ�Ҫ����������ָʾ����˸�����ţ�PTD7��
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================
#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

// �궨�������ڲ�ʹ�õĳ���
#define ONE_SECOND_DELAY  200



void SendHeartBeart(){
	if(net_status==REGISTERED){
		WPSendData("a",1,NZP_HEARTBEAT,PC_NODE_ADDR,0);
	}
}




//===========================================================================
//�������ƣ�task_light
//���ܸ�Ҫ����������ָʾ��ÿ��1s��˸�����������ȼ�Ϊ14����������task_adc
//����˵����δʹ��
//===========================================================================


void task_light(uint_32 initial)
{	
	// ��ʼ������
	uint_8 i = 0;
	uint_8 modNum = 0;
	while(TRUE) 
	{
////		uart_send1(UART_0,'a');
//		//���¼����û�����--------------------------------------------------------
////		//1����������ָʾ����˸
//		light_change(LIGHT_RUN_1);      //�ı�����ָʾ�����ŵ�ƽ״̬(���ţ�PTD7)
//		//2��������ʱ1��
//		_time_delay_ticks(0.5*ONE_SECOND_DELAY); //ÿ��tick��Ӧ5ms����ʱ200*5ms=1s
//		SendHeartBeart();
//		_time_delay_ticks(0.5*ONE_SECOND_DELAY); //ÿ��tick��Ӧ5ms����ʱ200*5ms=1s
//
//		SendHeartBeart();
		// ͨ�� light_control_value ��ȡ����ֵ
		i = light_control_value/25 + 1;
		if(i == 1)
			modNum = 8;
		if(i == 2)
			modNum = 4;
		if (i == 3)
			modNum = 2;
		if (i == 4)
			modNum = 1;
		// �� light_control_value Ϊ 0 �ǣ���Ϊ��
		if (light_control_value == 0)
			light_control(LIGHT_RUN_1, LIGHT_OFF);
		// �� light_control_value Ϊ 100 �ǣ���Ϊ��
		if (light_control_value == 100)
			light_control(LIGHT_RUN_1, LIGHT_ON);
		// ����ѭ�� delay 1��
		for (i = 0; i < 8; i++) {
			// delay 1/8s
			_time_delay_ticks(ONE_SECOND_DELAY/8);
			// ÿ���뷢һ��������
			if (i == 0 || i == 3) {
				SendHeartBeart();
			}
			// �� light_control_value Ϊ0��100ʱ����
			if (light_control_value == 0 || light_control_value == 100) {
				continue;
			}
			// ����С����˸
			if (i%modNum == 0) {
				light_change(LIGHT_RUN_1);
			}
		}
	}//����ѭ����end_while
}



