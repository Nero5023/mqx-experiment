//============================================================================
//�ļ���ƣ�wp.h
//���ܸ�Ҫ������ͨ�ż�����Э��ģ��
//============================================================================

#ifndef WP_H
#define WP_H
#include "Protocol.h"

#define MaxFrameLength 48

typedef enum NET_STATUS{
	UNREGISTERED=1, 			//��δע�� 		--> task_wp_register������ע����Ϣ
	REGISTERING, 				//ע����  			--> task_rf_recv����ȴ���շ���
	REGISTERING_WITH_ECHO,		//ע�������յ����� 	--> task_wp_register�����ж�ע���Ƿ�ɹ�
	REGISTERED 					//��ע�� 			--> task_wp_register������ֹ
}NET_STATUS;

NET_STATUS net_status;


void generateRegisterData();

//=====================================================================
//������ƣ�WPSendData
//����أ��޷���ֵ
//����˵����   data: ��Ҫ������ݵ��ַ�ָ��
//         length: ���͵���ݳ���
//       NZP_TYPE: ���� NZP Э�������
//    destination: ���͵�Ŀ�ĵ�
//  needEncrypted: �Ƿ���Ҫ����Ŀ�ĵ�
//���ܸ�Ҫ������һ֡ rf ���
//=====================================================================
void WPSendData(char *data, char length, enum NZP_TYPE type, char destination, char needEncrypted);


// ���� 0 ��ʾ����ʧ��
// ���� 1 ��ʾ���ͳɹ�
//=====================================================================
//������ƣ�WPSENDLargeData
//����أ�0 ��ʾ����ʧ��
//        1 ��ʾ���ͳɹ�
//����˵����   data: ��Ҫ������ݵ��ַ�ָ��
//         length: ���͵���ݳ���
//    totalLength: ���͵�����ݳ���, �����֡��
//    destination: ���͵�Ŀ�ĵ�
//            end: �Ƿ������
//���ܸ�Ҫ������һ֡ rf �����
//=====================================================================
uint_8 WPSENDLargeData(uint_8 *data, uint_8 length, uint_8 totalLength,char destination, uint_8 end);

void WPSENDLargeDataWithFrame(uint_8 *data, uint_8 length, char destination, uint_8 count);
#endif
