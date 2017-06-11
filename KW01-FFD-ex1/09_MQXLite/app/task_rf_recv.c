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
		_mqx_uint recv_msg[RECV_MSG_SIZE];//���ڽ���recv_queue�е���Ϣ

		_lwmsgq_receive((pointer)recv_queue,recv_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		uint_8 length = length_of_NZP((pointer)recv_msg);
		uint_8 data_length = data_length_of_NZP((pointer)recv_msg);


		char data[56];
		// ���� NZP Э�飬��������ɹ������͸��Լ��ģ�checksum ��ȷ��
		if (parse_NZP((pointer)recv_msg, length, data)) {
			// uart data
			// ��ȡ NZP ������
			NZP_TYPE type = type_of_NZP((pointer)recv_msg);
			// ��ȡ����Դ�ĵ�ַ
			uint_8 addr = addr_of_NZP((pointer)recv_msg);

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
					receive_heartbeat_from(addr);
					break;
				case NZP_HEARTBEAT:   // �յ�������
					receive_heartbeat_from(addr);
					break;
				case NZP_RTS:         // �յ� RTS �Ű�
					sendBigDataStart(addr, data[0]);
					break;
				case NZP_CTS:        // �յ� CTS �Ű�
					break;
				case NZP_TS_DATA:    // �յ������ݰ�
					sendBigData(data, data_length);
					break;
				case NZP_TS_END:    // �յ�������β֡
					// Lage_Data_Flag = CAN_NOT_SEND;
					uart_send_string(UART_0,"Send ACK");
					sendBigDataEnd();//uart send to pc
					break;
				case NZP_ACK:  // �յ� ack, �鿴�Ƿ���©��
					if(data[0]==0){
						break;
					}
					else{
						sendBigDataMiss(data);
					}
					break;
				default:
					break;
			}


		}
		
	}//����ѭ����end_while
}

