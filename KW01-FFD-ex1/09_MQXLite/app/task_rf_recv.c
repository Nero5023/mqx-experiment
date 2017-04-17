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

	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		//1�����޵ȴ�RF�����¼�λ��һ
		_lwevent_wait_for(&lwevent_group, EVENT_RF_RECV, FALSE, NULL);

		uint_8 length = length_of_NZP((pointer)rf_recvBuf);
		uint_8 data_length = data_length_of_NZP((pointer)rf_recvBuf);


		char data[56];
		// ���� NZP Э�飬��������ɹ������͸��Լ��ģ�checksum ��ȷ��
		if (parse_NZP((pointer)rf_recvBuf, length, data)) {
			// uart data
			// ��ȡ NZP ������
			NZP_TYPE type = type_of_NZP((pointer)rf_recvBuf);
			// ��ȡ����Դ�ĵ�ַ
			uint_8 addr = addr_of_NZP((pointer)rf_recvBuf);

			switch (type) {
				case NZP_REGISTER:// ��ע����Ϣ����
					_lwmsgq_send((pointer)register_queue,data,LWMSGQ_SEND_BLOCK_ON_FULL); //����ע����Ϣ������
					break;
				case NZP_DATA:// ��Ҫֱ�Ӵ�ӡ����Ϣ
					uart_sendN(UART_0, data_length, data);
					break;
				case NZP_TEMPERATURE: // �յ��¶���Ϣʱ��
					sendNodeTempInfo(addr,data);
					break;
				case NZP_REGISTER_Success: // �յ�ע��ɹ���Ϣ
					sendNodeReisterSuccessInfo(addr);
					break;
				default:
					break;
			}


		}
		
		//3��RF�����¼�λ����
		_lwevent_clear(&lwevent_group, EVENT_RF_RECV);
	}//����ѭ����end_while
}

