#include "01_app_include.h"
//===========================================================================
//�������ƣ�task_wp_register
//���ܸ�Ҫ�������Զ�������Э����������ע���Լ�
//===========================================================================


#define ONE_SECOND_DELAY 200
#define REGISTER_TIMEOUT 3

void task_wp_register(uint32_t initial){
	net_status = UNREGISTERED;
	uint_8 wait_time=0;
	while(TRUE){
		// uint_8 wait_time=0;
		switch (net_status) {
			case UNREGISTERED: //��δע����ʼע��
				if (SELF_ADDR == 0) {
					srand(0);
				    char key[4] = {rand() % 100};
				    key[1] = 'a';
				    key[2] = 'b';
				    key[3] = 'c';
				    ENCRYPT_KEY = key[0];
					WPSendData(key, 4, NZP_REGISTER, 0xff, 0);
					net_status = REGISTERING;//���״̬Ϊע����
				}
				break;

			case REGISTERING: //ע����,�ȴ��������ؽ��
				//��ʱ1��
				_time_delay_ticks(1*ONE_SECOND_DELAY); //ÿ��tick��Ӧ5ms����ʱ200*5ms=1s
				wait_time++;
				if(wait_time >= REGISTER_TIMEOUT){
					net_status = UNREGISTERED; //ע�ᳬʱ
					wait_time = 0;
				}
				break;
			case REGISTERING_WITH_ECHO:
			// 	//�ж��Ƿ�ע��ɹ�
				net_status = REGISTERED;
				break;
			case REGISTERED:
				//ע��ɹ�,��������
				return;
			default:
				break;
		}
	}
}
