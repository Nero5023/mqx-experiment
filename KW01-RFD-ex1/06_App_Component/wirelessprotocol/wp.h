//============================================================================
//�ļ����ƣ�wp.h
//���ܸ�Ҫ������ͨ�ż�����Э��ģ��
//============================================================================

#ifndef WP_H
#define WP_H
#include "Protocol.h"

typedef enum NET_STATUS{
	UNREGISTERED=1, 			//��δע�� 		--> task_wp_register��������ע����Ϣ
	REGISTERING, 				//ע����  			--> task_rf_recv�����ȴ����շ���
	REGISTERING_WITH_ECHO,		//ע�������յ����� 	--> task_wp_register�����ж�ע���Ƿ�ɹ�
	REGISTERED 					//��ע�� 			--> task_wp_register������ֹ
}NET_STATUS;

NET_STATUS net_status;


void generateRegisterData();
void WPSendData(char *data, char length, enum NZP_TYPE type, char destination, char needEncrypted);

#endif
