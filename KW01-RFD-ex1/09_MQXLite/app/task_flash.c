//===========================================================================
//�ļ����ƣ�task_flash.c
//���ܸ�Ҫ��FLASH����(����д������)
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================

#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�
uint_8  bufRetErr[18]  = {'M',0x15,'F','l','a','s','h','-','x','x','x','x','x','-','E','r','r'};
//===========================================================================
//�������ƣ�task_flash
//���ܸ�Ҫ���ж�FLASH�¼�λEVENT_FLASH������������(R_��  W_д E_����)ִ����Ӧ�Ĳ�����
//       ������ɣ��������Ľ��ͨ��UART0���͵����ڣ����EVENT_FLASH
//����˵����δʹ��
//===========================================================================
void task_flash(uint32_t initial)
{	
	//1. ��������ʹ�õı���
	int i;
	int offset,count;
	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		int i;
		uint_8 sector;

		//1�����޵ȴ�RF�����¼�λ��һ
		_lwevent_wait_for(&lwevent_group, EVENT_FLASH, FALSE, NULL);

		//2�����ý��պ���
		//��ȡflashָ����������
		switch(g_uart_recvBuf[3]){
		case 'R':
			offset = g_uart_recvBuf[5] *256+g_uart_recvBuf[6];
			count  = g_uart_recvBuf[7];

			g_uart_sentBuf[0]  =(uint_8)'M';           //
			g_uart_sentBuf[1]  =count+1;               //
			g_uart_sentBuf[2]  = (uint_8)'C';          //
			flash_read(g_uart_recvBuf[4],offset,count,&g_uart_sentBuf[3]);
			g_uart_sentBuf[count+3]  = (uint_8)'U';  //

			uart_sendN(UART_0,count+4,&g_uart_sentBuf[0]);
			break;

		case 'W':
			offset =g_uart_recvBuf[5]*256+g_uart_recvBuf[6];
			count = g_uart_recvBuf[7];

			g_uart_sentBuf[0]=(uint_8)'M';     //
			g_uart_sentBuf[1]=count+1;         //
			g_uart_sentBuf[2]= (uint_8)'C';    //
			flash_write(g_uart_recvBuf[4], offset, count, &g_uart_recvBuf[8]);
			flash_read(g_uart_recvBuf[4], offset, count, &g_uart_sentBuf[3]);
			g_uart_sentBuf[count+3]  = (uint_8)'U';  //

			uart_sendN(UART_0,count+4, &g_uart_sentBuf[0]);
			break;

		case 'E':
			DISABLE_INTERRUPTS;//���ж�
			if(flash_erase(g_uart_recvBuf[4]) == 0 )
			{
				g_uart_sentBuf[0]=(uint_8)'M';     //
				g_uart_sentBuf[1]=32+1;            //
				g_uart_sentBuf[2]= (uint_8)'C';    //
				flash_read(g_uart_recvBuf[4], 0, 32, &g_uart_sentBuf[3]);
				g_uart_sentBuf[g_uart_sentBuf[1]+2]  = (uint_8)'U';
				uart_sendN(UART_0,g_uart_sentBuf[1]+3, &g_uart_sentBuf[0]);
			}
			else
			{
				uart_sendN(UART_0,18,&bufRetErr[0]);
			}
			ENABLE_INTERRUPTS;//���ж�
		}

		//3������¼�λ
		_lwevent_clear(&lwevent_group, EVENT_FLASH);
	}//����ѭ����end_while
}

