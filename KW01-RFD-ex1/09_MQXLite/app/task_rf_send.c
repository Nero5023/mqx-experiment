//===========================================================================
//�ļ����ƣ�task_rf_send.c
//���ܸ�Ҫ������RF��������֡����
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================
#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

//===========================================================================
//�������ƣ�task_rf_send
//���ܸ�Ҫ���ж�RF�����¼�λEvent_RF_Send������RF_SendData()����ת�����ղ������ݰ���
//����˵����δʹ��
//===========================================================================
void task_rf_send(uint32_t initial)
{	
	//1. ��������ʹ�õı���
	uint_8 i;


	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		//1�����޵ȴ�RF�����¼�λ��һ
		_lwevent_wait_for(&lwevent_group, EVENT_RF_SEND, FALSE, NULL);

		/*
		rf_sentDataLen = g_uart_recvBuf[1]-1;  //��ҪRFת�����ֽ���

		for (i=0;i<rf_sentDataLen;i++) rf_sentBuf[i]=g_uart_recvBuf[i+3];


		//2��RFģ�鷢������,������������������ŵ�������ֵ
		if(g_uart_recvBuf[58]=='N')
	        RFSendDataByCSMACA(rf_sentDataLen,&rf_sentBuf[0],0,HD_adr);
		else
	        RFSendDataByCSMACA(rf_sentDataLen,&rf_sentBuf[0],1,HD_adr);
		*/
//		RFSendDataByCSMACA(15,"An RF test msg.",0,HD_adr);
//		uart_send_string(UART_0,"Sended a test frame.\n");

		RFSendDataByCSMACA(rf_sentDataLength,rf_sentBuf,0,255);

		//���RF�����¼�λ
		_lwevent_clear(&lwevent_group, EVENT_RF_SEND);

	}//����ѭ����end_while
}
