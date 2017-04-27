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
	char key[4];
	while(TRUE){
		// uint_8 wait_time=0;
		switch (net_status) {
			case UNREGISTERED: //��δע����ʼע��
				if (SELF_ADDR == 0) {  // �鿴�Ƿ����δע��
					srand(123);
				    key[0]=(rand() % 100+1);
				    // 1-3 Ϊ������ݣ���ȫΪ 4 ���ֽ�
				    key[1] = 'a';
				    key[2] = 'b';
				    key[3] = 'c';
				    ENCRYPT_KEY = key[0];

				    // �㲥�Լ�����Կ�����͸������ڵ�
					WPSendData(key, 4, NZP_REGISTER, BROADCAST_ADDR, 0);
					net_status = REGISTERING;//���״̬Ϊע����
				}
				break;

			case REGISTERING: //ע����,�ȴ��������ؽ��
				//��ʱ1�� ÿ��tick��Ӧ5ms����ʱ200*5ms=1s
				_time_delay_ticks(1*ONE_SECOND_DELAY); 
				wait_time++;
				if(wait_time >= REGISTER_TIMEOUT){ // ��ʱ
					net_status = UNREGISTERED; //ע�ᳬʱ
					wait_time = 0;
				}
				break;
			case REGISTERING_WITH_ECHO:
			 	//�ж��Ƿ�ע��ɹ�
				net_status = REGISTERED;
				break;
			case REGISTERED:
				//ע��ɹ�,��������������ע��ɹ���Ϣ
				WPSendData("RS",2,NZP_REGISTER_Success,BROADCAST_ADDR,0);
				return;
			default:
				break;
		}
	}
}
