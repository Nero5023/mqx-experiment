#include "01_app_include.h"
//===========================================================================
//�������ƣ�task_wp_register
//���ܸ�Ҫ�������Զ�������Э����������ע���Լ�
//===========================================================================


#define ONE_SECOND_DELAY 200

void task_wp_register(uint32_t initial){
	generateRegisterData();		//����ע����Ϣ
	while(TRUE){
		//������ʱ3��
		_time_delay_ticks(3*ONE_SECOND_DELAY); //ÿ��tick��Ӧ5ms����ʱ200*5ms=1s

		if(net_status==UNREGISTERED){
			HD_adr=255;
		    RF_Init(HD_adr);		    //RFģ���ʼ��
			_lwevent_set(&lwevent_group,EVENT_RF_SEND);	//��RF�����¼�λ��EVENT_RF_SEND��������task_rf_send����
		}else{
			return;
		}
	}
}
