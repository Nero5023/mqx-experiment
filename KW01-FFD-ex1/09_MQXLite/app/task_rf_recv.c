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
//		uart_send_string(UART_0,"Got a msg from RF.\n");
		// uart_sendN(UART_0,64,rf_recvBuf);
		//���ڷ�������ȫ������
//		g_uart_sentBuf[0]='M';    //77
//
//		g_uart_sentBuf[1]=g_rfRecCount+1;
//
//		g_uart_sentBuf[2]='B';                    //66
//
//		for (i=3;i<=g_uart_sentBuf[1]+1;i++)	g_uart_sentBuf[i]=rf_recvBuf[i-3];
//
//		g_uart_sentBuf[g_uart_sentBuf[1]+2]='U';   //85=U

		//2�����ý��պ���
		//rf_recvBuf[60]=(uint_16)g_temperature;   		     //��������
		//rf_recvBuf[61]=(g_temperature-rf_recvBuf[60])*10+0.5;//С������(����һλС������������)
		//rf_recvBuf[62]=RFEnergyDetect();

//uart_sendN(UART_0,g_uart_sentBuf[1]+3,&g_uart_sentBuf[0]);              //ͨ�����Դ���0��������

//		for (i=0;i<rf_recvBuf[1]+3;i++)	g_uart_sentBuf[i]=rf_recvBuf[i];
//		g_uart_sentBuf[i]='\0';
//		uart_sendN(UART_0,rf_recvBuf[1]+3,&g_uart_sentBuf[0]);
//		uart_send_string(UART_0,rf_recvBuf);

		uint_8 length = length_of_NZP(rf_recvBuf);
		uint_8 data_length = data_length_of_NZP(rf_recvBuf);
		char data[56];
		if (parse_NZP(rf_recvBuf, length, data)) {
			// uart data
			NZP_TYPE type = type_of_NZP(rf_recvBuf);
			uart_sendN(UART_0, data_length, data);
		}
		
		//3��RF�����¼�λ����
		_lwevent_clear(&lwevent_group, EVENT_RF_RECV);
	}//����ѭ����end_while
}

