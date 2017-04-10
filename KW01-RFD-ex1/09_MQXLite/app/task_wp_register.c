#include "01_app_include.h"
//===========================================================================
//�������ƣ�task_wp_register
//���ܸ�Ҫ�������Զ�������Э����������ע���Լ�
//===========================================================================


#define ONE_SECOND_DELAY 200
#define REGISTER_TIMEOUT 3

void task_wp_register(uint32_t initial){
	generateRegisterData();		//����ע����Ϣ
	while(TRUE){
		uint_8 wait_time=0;
		switch (net_status) {
			case UNREGISTERED: //��δע����ʼע��

				HD_adr=255;
				RF_Init(HD_adr);		    //RFģ���ʼ��
				_lwevent_set(&lwevent_group,EVENT_RF_SEND);	//��RF�����¼�λ��EVENT_RF_SEND��������task_rf_send����
				net_status = REGISTERING;//���״̬Ϊע����

				break;

			case REGISTERING: //ע����,�ȴ��������ؽ��
				//��ʱ1��
				_time_delay_ticks(1*ONE_SECOND_DELAY); //ÿ��tick��Ӧ5ms����ʱ200*5ms=1s
				wait_time++;
				if(wait_time >= REGISTER_TIMEOUT){
					net_status = UNREGISTERED; //ע�ᳬʱ
					wait_time = 0;
				}

			case REGISTERING_WITH_ECHO:
				//�ж��Ƿ�ע��ɹ�

				break;
			case REGISTERED:
				//ע��ɹ�,��������
				return;
			default:
				break;
		}
	}
}
