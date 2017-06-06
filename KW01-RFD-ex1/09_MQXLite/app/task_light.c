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
	//const uint32_t X1=0x01020304;
	//const uint32_t X1=0x01020304 @0x30004000;
	// ��������ѭ����
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

		i = light_control_value/25 + 1;
		if(i == 1)
			modNum = 8;
		if(i == 2)
			modNum = 4;
		if (i == 3)
			modNum = 2;
		if (i == 4)
			modNum = 1;
		if (light_control_value == 0)
			light_control(LIGHT_RUN_1, LIGHT_OFF);
		if (light_control_value == 100)
			light_control(LIGHT_RUN_1, LIGHT_ON);
		for (i = 0; i < 8; i++) {
			_time_delay_ticks(ONE_SECOND_DELAY/8);
			if (i == 0 || i == 3) {
				SendHeartBeart();
			}
			if (light_control_value == 0 || light_control_value == 100) {
				continue;
			}
			if (i%modNum == 0) {
				light_change(LIGHT_RUN_1);
			}
		}
	}//����ѭ����end_while
}



