//===========================================================================
//�ļ����ƣ�task_flash.c
//���ܸ�Ҫ��FLASH����(����д������)
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================

#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�
#define FILE_INFO_SECTOR 109
#define FLASH_START_SECTOR 110
uint_8  bufRetErr[18]  = {'M',0x15,'F','l','a','s','h','-','x','x','x','x','x','-','E','r','r'};


void getCurrentSectorAndOffset(uint_8 frameOrder,uint_8* sector, uint_8* offset){
	uint_8 cycle = 1024/MaxFrameLength;
	uint_8 iterCount = frameOrder/cycle;

	uint_8 lastSectorOffset = frameOrder%cycle;

	*sector = iterCount;
	*offset = lastSectorOffset;

}



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
	int offset=0;
	uint_8 sector = FLASH_START_SECTOR;
	uint_8 total_len = 0;
	uint_8 frameOrder= 0;
	uint_8 flash_write_temp[FLASH_WRITE_MSG_SIZE*4];

	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		int i;

		//1�����޵ȴ�RF������Ϣ
		_lwmsgq_receive((pointer)flash_write_queue,flash_write_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		//2�����ý��պ���
		//��ȡflashָ����������
		switch(flash_write_temp[0]){

		case 'B':
			//��ʼд��flash,��¼����֡��
			total_len = flash_write_temp[1];
			flash_erase(FILE_INFO_SECTOR);
			flash_write(FILE_INFO_SECTOR,0,1,total_len);
			break;

		case 'R':
			flash_read(FILE_INFO_SECTOR,0,1,&total_len); //��flash�����ļ�����
			i=1;
			while(i<total_len){
				getCurrentSectorAndOffset(frameOrder,&sector,&offset); //����frameOrder�����Ӧ�������ź�ƫ����
				flash_read(FLASH_START_SECTOR+sector,offset*52,MaxFrameLength,flash_write_temp);
				WPSENDLargeData(flash_write_temp,MaxFrameLength,total_len,PC_NODE_ADDR,0);
				i++;
			}
			WPSENDLargeData("",MaxFrameLength,total_len,PC_NODE_ADDR,1); // ���ͽ���֡

			break;

		case 'W':
			DISABLE_INTERRUPTS;//���ж�

			frameOrder = flash_write_temp[1];

			getCurrentSectorAndOffset(frameOrder,&sector,&offset); //����frameOrder�����Ӧ�������ź�ƫ����

			flash_write(FLASH_START_SECTOR+sector,offset*52,MaxFrameLength,((char*)flash_write_temp)+1);


			ENABLE_INTERRUPTS;//���ж�
			break;
		case 'S':
			//д�����
			sector = FLASH_START_SECTOR;
			offset = 0;
		case 'E':
			DISABLE_INTERRUPTS;//���ж�
//			if(flash_erase(g_uart_recvBuf[4]) == 0 )
//			{
//				g_uart_sentBuf[0]=(uint_8)'M';     //
//				g_uart_sentBuf[1]=32+1;            //
//				g_uart_sentBuf[2]= (uint_8)'C';    //
//				flash_read(g_uart_recvBuf[4], 0, 32, &g_uart_sentBuf[3]);
//				g_uart_sentBuf[g_uart_sentBuf[1]+2]  = (uint_8)'U';
//				uart_sendN(UART_0,g_uart_sentBuf[1]+3, &g_uart_sentBuf[0]);
//			}
//			else
//			{
//				uart_sendN(UART_0,18,&bufRetErr[0]);
//			}
			ENABLE_INTERRUPTS;//���ж�
		}

	}//����ѭ����end_while
}

