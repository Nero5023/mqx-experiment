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
		_mqx_uint recv_msg[RECV_MSG_SIZE];//���ڽ���recv_queue�е���Ϣ

		//���¼����û�����--------------------------------------------------------
		//1�����޵ȴ�RF�����¼�λ��һ
		_lwmsgq_receive((pointer)recv_queue,recv_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		uint_8 length = length_of_NZP(recv_msg);
		uint_8 data_length = data_length_of_NZP(recv_msg);
		char data[56];
		uint_8 total_len;
		uint_8 frameOrder;
		uint_8 flash_send_temp[FLASH_WRITE_MSG_SIZE*4];

		uint_8 received_data[256];
		uint_8 i;
		uint_8 not_received_data[256];
		uint_8 not_received_count;

		// ���� NZP Э�飬��������ɹ������͸��Լ��ģ�checksum ��ȷ��
		if (parse_NZP(recv_msg, length, data)) {

//			// ���� debug
//			uart_sendN(UART_0,data_length,data);

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
				case NZP_RTS:
					uart_send_string(UART_0,"write begin\r\n");
					total_len =  data[0];
					data[0]='B'; //begin of flash write
					data[1]=total_len;
					for (i = 0; i < total_len; i++) {
						received_data[i] = 0;
					}
					not_received_count = 0;
					_lwmsgq_send((pointer)flash_write_queue,data,LWMSGQ_SEND_BLOCK_ON_FULL);
					break;
				case NZP_CTS:
					//not use
					break;
				case NZP_TS_DATA:
//					uart_send_string(UART_0,"write data");
					uart_sendN(UART_0,4,data);
					frameOrder = data[0];
					flash_send_temp[0]='W';
					flash_send_temp[1]=frameOrder; //�������,�������ֱ�Ӵ����Ӧλ��
					flash_send_temp[2]=data_length-1;
					received_data[frameOrder] = 1;
					memcpy(flash_send_temp+3,data+1,data_length-1); //���һ֡���ܿ�����������
					_lwmsgq_send((pointer)flash_write_queue,flash_send_temp,LWMSGQ_SEND_BLOCK_ON_FULL);
					break;
				case NZP_TS_END:
					uart_send_string(UART_0,"write end\r\n");
					Lage_Data_Flag = CAN_NOT_SEND;

					for (i = 0; i < total_len;i++) {
						if (received_data[i] == 0) {
							not_received_count+=1;
							not_received_data[not_received_count] = i;
						}
					}
					not_received_data[0] = not_received_count; // ��һλ û�յ���֡��x��֮��x��Ϊû�յ���֡
					WPSendData(not_received_data, not_received_count+1, NZP_ACK, PC_NODE_ADDR, 0);
					not_received_count = 0;
					data[0]='S';//д�����
					_lwmsgq_send((pointer)flash_write_queue,data,LWMSGQ_SEND_BLOCK_ON_FULL);
					break;
				case NZP_ACK:
					uart_send_string(UART_0,"Recv ACK\r\n");
					break;
				case NZP_DATA_READ:
					data[0]='R';
					_lwmsgq_send((pointer)flash_write_queue,data,LWMSGQ_SEND_BLOCK_ON_FULL);
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

