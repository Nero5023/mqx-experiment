//============================================================================
//�ļ����ƣ�wp.h
//���ܸ�Ҫ������ͨ�ż�����Э��ģ��
//============================================================================

#ifndef WP_H
#define WP_H


typedef enum NET_STATUS{
	UNREGISTERED=1, //��δע��
	REGISTERING, //ע����
	REGISTERED //��ע��
}NET_STATUS;

static NET_STATUS net_status = UNREGISTERED;


void generateRegisterData();

#endif
