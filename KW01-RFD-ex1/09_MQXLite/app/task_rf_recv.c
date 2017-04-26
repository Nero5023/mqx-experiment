//===========================================================================
//�ļ����ƣ�task_rf_recv.c
//���ܸ�Ҫ������RF��������֡����
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================

#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�


//===========================================================================
//�������ƣ�task_rf_recv
//���ܸ�Ҫ���ж�RF�����¼�λEVENT_RF_RECV������RF_ReceiveFrame()����PC Node�ڵ�RF�������ݰ���
//       �����ɹ���������ڵ�оƬ�¶��Լ�RF����ֵ��������������ݵ���������ֽ��У�����RF�����������¼�λEVENT_RF_SEND��
//       ����RF��������task_rf_send()��
//����˵����δʹ��
//===========================================================================
void task_rf_recv(uint32_t initial)
{	
	//1. ��������ʹ�õı���
	uint_8 i;
	_mqx_int recv_msg[RECV_MSG_SIZE];//���ڽ���recv_queue�е���Ϣ

	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		//1�����޵ȴ�RF�����¼�λ��һ
		_lwmsgq_receive((pointer)recv_queue,recv_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);


		uint_8 length = length_of_NZP(recv_msg);
		uint_8 data_length = data_length_of_NZP(recv_msg);
		char data[56];

		// ���� NZP Э�飬��������ɹ������͸��Լ��ģ�checksum ��ȷ��
		if (parse_NZP(recv_msg, length, data)) {

			// ���� debug
			uart_sendN(UART_0,data_length,data);

			// ��ȡ���ݱ�Э������
			NZP_TYPE type = type_of_NZP(recv_msg);

			char clearText[2];
			switch (type) {
				case NZP_REGISTER_Success:       // ���յ�ע��ظ���Ϣ
					// ��������

					decode(data, clearText, 2, ENCRYPT_KEY);
					if (clearText[0] == ENCRYPT_KEY && net_status == REGISTERING) { // ������Ϣ�е���Կ���Լ���Կ��ͬʱ����Ϊ���͸��Լ���
						SELF_ADDR = clearText[1];  // �����Լ��ĵ�ַ
						net_status = REGISTERING_WITH_ECHO;  // ��ע��״̬��Ϊ REGISTERING_WITH_ECHO
					}
					break;
				case NZP_TEMPERATURE:
					// ���Ͷ�Ӧ��ȫ�ֱ���
					WPSendData(&g_temperature,4,NZP_TEMPERATURE,PC_NODE_ADDR,0);
					break;
				case NZP_CONTIONUOUS_MONITOR:
					if(is_continous_monitoring==0){
						is_continous_monitoring=1;
					}else{
						is_continous_monitoring=0;
					}
					break;
				default:
					break;
			}
		}
		else{
			// ���� Debug
			uart_send_string(UART_0,"Parse Failed.");
		}

	}//����ѭ����end_while
}

