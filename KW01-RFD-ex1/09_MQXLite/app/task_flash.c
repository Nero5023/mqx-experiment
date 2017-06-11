//===========================================================================
//�ļ����ƣ�task_flash.c
//���ܸ�Ҫ��FLASH����(����д������)
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�������Ҫ����
//===========================================================================

#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�
#define FILE_INFO_SECTOR 109
#define FLASH_START_SECTOR 30
uint_8  bufRetErr[18]  = {'M',0x15,'F','l','a','s','h','-','x','x','x','x','x','-','E','r','r'};


//===========================================================================
//�������ƣ�getCurrentSectorAndOffset
//���ܸ�Ҫ�������ڲ����������ڸ���frameOrder��ȡ��Ӧ�������ź�ƫ����
//����˵����
//		frameOrder��֡���кţ�
//		sector�����ڷ���������
//      offset�����ڷ���ƫ����
//===========================================================================
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
	uint_8 flash_read_temp[FLASH_WRITE_MSG_SIZE*4];
	uint_8 write_len=0;

	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE) 
	{
		//���¼����û�����--------------------------------------------------------
		int i;
//		uart_send_string(UART_0,"FlashTB");
		//1�����޵ȴ�flash������Ϣ����
		_lwmsgq_receive((pointer)flash_write_queue,flash_write_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);
//		uart_send_string(UART_0,"FlashTA");
		//2�����ݽ��յ���flash��Ϣ�ĵ�һ��ֱ���ж϶�ȡִ��ʲô����
		switch(flash_write_temp[0]){

		case 'B':
			//��ʼд��flash,��¼����֡��
			total_len = flash_write_temp[1];
			flash_erase(FILE_INFO_SECTOR);
			flash_write(FILE_INFO_SECTOR,0,1,&total_len);
			getCurrentSectorAndOffset(total_len-1,&sector,&offset);
			for(i=0;i<=sector;i++){
				flash_erase(FLASH_START_SECTOR+sector);
			}
			break;

		case 'R':
			//��ȡ����
//			uart_send_string(UART_0,"read begin\n");
			flash_read(FILE_INFO_SECTOR,0,1,&total_len); //��flash�����ļ�����
			i=0;
//			WPSendData(&total_len, 1, NZP_RTS, PC_NODE_ADDR, 0);
			frameOrder = 0;
			while(i<total_len){
				 //����frameOrder�����Ӧ�������ź�ƫ����
				getCurrentSectorAndOffset(i,&sector,&offset);
				 //����һ��frame
				flash_read(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_read_temp);
				//����һ��frame
				WPSENDLargeData(flash_read_temp,MaxFrameLength,total_len,PC_NODE_ADDR,0);

				i++;

			}
			 // ���ͽ���֡
			WPSENDLargeData(" ",MaxFrameLength,total_len,PC_NODE_ADDR,1);
			break;
		case 'W':
			//д�����
			DISABLE_INTERRUPTS;//���ж�
			//֡���к�
			frameOrder = flash_write_temp[1];
			//д�볤��
			write_len = flash_write_temp[2];
			//����frameOrder�����Ӧ�������ź�ƫ����
			getCurrentSectorAndOffset(frameOrder,&sector,&offset);
			//д��һ��frame
			flash_write(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_write_temp+3);
			//��ӡ������Ϣ
//			uart_send1(UART_0, frameOrder/10+'0');
//			uart_send1(UART_0, frameOrder%10+'0');
//			uart_send_string(UART_0,"frame\r\n");
			ENABLE_INTERRUPTS;//���ж�
			break;
		case 'S':
			//д�����
			sector = 0;
			offset = 0;
			break;
		case 'M':
			//���¶�ȡ��ʧ��frame
			DISABLE_INTERRUPTS;//���ж�
			//��ӡ������Ϣ
//			uart_send1(UART_0, flash_write_temp[1]/10+'0');
//			uart_send1(UART_0, flash_write_temp[1]%10+'0');
//			uart_send_string(UART_0," frames pc feel miss \r\n");

			//flash_write_temp[0]:'M'
			//flash_write_temp[1]:��ʧ�ĸ���
			//flash_write_temp[2~2+��ʧ�ĸ���]:��ʧ��frameOrder
			//��ÿ����ʧ��frame���·���
			for(i=0;i<flash_write_temp[1];i++){
				frameOrder = flash_write_temp[2+i];
				getCurrentSectorAndOffset(frameOrder,&sector,&offset); //����frameOrder�����Ӧ�������ź�ƫ����
				flash_read(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_read_temp);
				WPSENDLargeDataWithFrame(flash_read_temp,MaxFrameLength,PC_NODE_ADDR,frameOrder);
			}
			//���ͽ�����Ϣ
			WPSendData("a", 1, NZP_TS_END, PC_NODE_ADDR, 0);
			ENABLE_INTERRUPTS;//���ж�
			break;

		default:
			break;
		}

	}//����ѭ����end_while
}

