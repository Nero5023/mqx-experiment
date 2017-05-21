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
		uart_send_string(UART_0,"FlashTB");
		//1�����޵ȴ�RF������Ϣ
		_lwmsgq_receive((pointer)flash_write_queue,flash_write_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);
		uart_send_string(UART_0,"FlashTA");
		//2�����ý��պ���
		//��ȡflashָ����������
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
			uart_send_string(UART_0,"read begin");
			flash_read(FILE_INFO_SECTOR,0,1,&total_len); //��flash�����ļ�����
			i=0;
//			WPSendData(&total_len, 1, NZP_RTS, PC_NODE_ADDR, 0);
			while(i<total_len){
				getCurrentSectorAndOffset(frameOrder,&sector,&offset); //����frameOrder�����Ӧ�������ź�ƫ����
				flash_read(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_read_temp+1);
//				uart_sendN(UART_0,3,flash_read_temp);
				WPSENDLargeData(flash_read_temp,MaxFrameLength,total_len,PC_NODE_ADDR,0);
//				flash_read_temp[0]=i;
//				WPSendData(flash_read_temp,MaxFrameLength,NZP_TS_DATA,PC_NODE_ADDR,0);
//				WPSENDLargeDataWithFrame(flash_read_temp, MaxFrameLength, PC_NODE_ADDR, i);
				//WPSendData('1',1, NZP_DATA, PC_NODE_ADDR, 0);
//				uart_send_string(UART_0,"read after data");
				i++;
//				_time_delay_ticks(100);
			}
//			WPSendData("a",1,NZP_TS_END,PC_NODE_ADDR,0);
			WPSENDLargeData(" ",MaxFrameLength,total_len,PC_NODE_ADDR,1); // ���ͽ���֡
			uart_send_string(UART_0,"read end 0");
			break;
;
		case 'W':
			DISABLE_INTERRUPTS;//���ж�

			frameOrder = flash_write_temp[1];
			write_len = flash_write_temp[2];
			uart_send_string(UART_0,"Data to write:");
			uart_sendN(UART_0,5,flash_write_temp+3);
			getCurrentSectorAndOffset(frameOrder,&sector,&offset); //����frameOrder�����Ӧ�������ź�ƫ����
			uart_send_string(UART_0,"Sector:");
			uart_send1(UART_0,sector+'0');
			uart_send_string(UART_0,"Offset:");
			uart_send1(UART_0,offset+'0');
			flash_write(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_write_temp+3);


			ENABLE_INTERRUPTS;//���ж�
			break;
		case 'S':
			//д�����
			sector = 0;
			offset = 0;
			break;
		default:
			break;
		}

	}//����ѭ����end_while
}

