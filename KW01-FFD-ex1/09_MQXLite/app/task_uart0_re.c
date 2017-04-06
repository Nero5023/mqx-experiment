//===========================================================================
//�ļ����ƣ�task_uart0_re.c
//���ܸ�Ҫ������Uart0��������֡�Ľ�֡����
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================
#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

extern uint_8 HW_ADR;
//===========================================================================
//�������ƣ�task_uart0_re
//���ܸ�Ҫ���жϴ��н����¼�λEvent_Uart0_Re���������н�������֡����������֡���ݾ�����1���������źţ�
//       2ֱ�Ӵ��ڻ��ͽ�����Ϣ��3����RF�����������¼�λEvent_RF_Send������RF��������task_rf_send()��
//����˵����δʹ��
//===========================================================================
void task_uart0_re(uint32_t initial)
{	
	//1. ��������ʹ�õı���
	uint_16 i;
	uint_16 offset;
	uint_16 count;
	uint_16 ret;	//��������ֵ
	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		//1�����޵ȴ�������0������������֡�¼�λ��
		_lwevent_wait_for(&lwevent_group, EVENT_UART0_RE, FALSE, NULL);

		DISABLE_INTERRUPTS;
		// ����Ӳ�����˵�ַ���ж�֡���ͣ�����֡����ת��֡��
		uart_send_string(UART_0,"Got a frame from UART0.\n");
		if(g_uart_recvBuf[2] == 0)    //��������
	    	{
	    		g_uart_sentBuf[0]=(uint_8)'M';     //
	    		g_uart_sentBuf[1]=7;               //
	    		g_uart_sentBuf[2]  = (uint_8)'C';  //
	    		g_uart_sentBuf[3]  = (uint_8)'P';  //"PCNode"
	    		g_uart_sentBuf[4]  = (uint_8)'C';
	    		g_uart_sentBuf[5]  = (uint_8)'N';
	    		g_uart_sentBuf[6]  = (uint_8)'o';
	    		g_uart_sentBuf[7]  = (uint_8)'d';
	    		g_uart_sentBuf[8]  = (uint_8)'e';
	    		g_uart_sentBuf[9]  = (uint_8)'U';  //
				uart_sendN(UART_0,10,&g_uart_sentBuf[0]);
	    	}
		else      // ����ת��
		{
			HD_adr=g_uart_recvBuf[2];   //HD_adrΪȫ�ֱ�����Ӳ����ַ
		    RF_Init(HD_adr);		    //RFģ���ʼ��
			 _lwevent_set(&lwevent_group,EVENT_RF_SEND);	//��RF�����¼�λ��EVENT_RF_SEND��������task_rf_send����
		}
//		//2����֡g_uart_recvBuf,g_uart_recvBuf[2]�������ֽ�
//		switch(g_uart_recvBuf[2])
//		{
//		case 'A':    //ֱ�ӷ��Ͳ������ݰ�
//			g_uart_recvBuf[2]=60;
//			uart_sendN(UART_0,60,&g_uart_recvBuf[3]);
//			break;
//		case 'B':    //RFת���������ݰ�
//		    _lwevent_set(&lwevent_group,EVENT_RF_SEND);	//��RF�����¼�λ��EVENT_RF_SEND��������task_rf_send����
//			break;
//			//���ִ������ISR�д���
//		case 'D':
//			_lwevent_set(&lwevent_group,EVENT_FLASH);	//��Flash�¼�λ������Flash��������
//			break;
//		case 'E':
//			HD_adr=g_uart_recvBuf[9];   //HD_adrΪȫ�ֱ�����Ӳ����ַ
//		    RF_Init(HD_adr);		         //RFģ���ʼ��
//			uart_sendN(UART_0,1,&g_uart_recvBuf[9]);
//			break;
//		default:
//			break;
//		}

		ENABLE_INTERRUPTS;

        //3��Uart0�����¼�λ����
		_lwevent_clear(&lwevent_group, EVENT_UART0_RE);
	}//����ѭ����end_while
}
