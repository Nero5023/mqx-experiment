//===========================================================================
//文件名称：task_flash.c
//功能概要：FLASH操作(读、写、擦除)
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================

#include "01_app_include.h"    //应用任务公共头文件
#define FILE_INFO_SECTOR 109
#define FLASH_START_SECTOR 30
uint_8  bufRetErr[18]  = {'M',0x15,'F','l','a','s','h','-','x','x','x','x','x','-','E','r','r'};


//===========================================================================
//任务名称：getCurrentSectorAndOffset
//功能概要：任务内部函数，用于根据frameOrder获取对应的扇区号和偏移量
//参数说明：
//		frameOrder：帧序列号，
//		sector：用于返回扇区号
//      offset：用于返回偏移量
//===========================================================================
void getCurrentSectorAndOffset(uint_8 frameOrder,uint_8* sector, uint_8* offset){
	uint_8 cycle = 1024/MaxFrameLength;
	uint_8 iterCount = frameOrder/cycle;

	uint_8 lastSectorOffset = frameOrder%cycle;

	*sector = iterCount;
	*offset = lastSectorOffset;

}



//===========================================================================
//任务名称：task_flash
//功能概要：判断FLASH事件位EVENT_FLASH，根据命令字(R_读  W_写 E_擦除)执行相应的操作，
//       操作完成，将操作的结果通过UART0发送到串口，清除EVENT_FLASH
//参数说明：未使用
//===========================================================================
void task_flash(uint32_t initial)
{	
	//1. 声明任务使用的变量
	int i;
	int offset=0;
	uint_8 sector = FLASH_START_SECTOR;
	uint_8 total_len = 0;
	uint_8 frameOrder= 0;
	uint_8 flash_write_temp[FLASH_WRITE_MSG_SIZE*4];
	uint_8 flash_read_temp[FLASH_WRITE_MSG_SIZE*4];
	uint_8 write_len=0;

	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		int i;
//		uart_send_string(UART_0,"FlashTB");
		//1）无限等待flash操作消息到来
		_lwmsgq_receive((pointer)flash_write_queue,flash_write_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);
//		uart_send_string(UART_0,"FlashTA");
		//2）根据接收到的flash消息的第一个直接判断读取执行什么操作
		switch(flash_write_temp[0]){

		case 'B':
			//开始写入flash,记录下总帧数
			total_len = flash_write_temp[1];
			flash_erase(FILE_INFO_SECTOR);
			flash_write(FILE_INFO_SECTOR,0,1,&total_len);
			getCurrentSectorAndOffset(total_len-1,&sector,&offset);
			for(i=0;i<=sector;i++){
				flash_erase(FLASH_START_SECTOR+sector);
			}
			break;

		case 'R':
			//读取操作
//			uart_send_string(UART_0,"read begin\n");
			flash_read(FILE_INFO_SECTOR,0,1,&total_len); //从flash读出文件长度
			i=0;
//			WPSendData(&total_len, 1, NZP_RTS, PC_NODE_ADDR, 0);
			frameOrder = 0;
			while(i<total_len){
				 //根据frameOrder计算对应的扇区号和偏移量
				getCurrentSectorAndOffset(i,&sector,&offset);
				 //读出一个frame
				flash_read(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_read_temp);
				//发送一个frame
				WPSENDLargeData(flash_read_temp,MaxFrameLength,total_len,PC_NODE_ADDR,0);

				i++;

			}
			 // 发送结束帧
			WPSENDLargeData(" ",MaxFrameLength,total_len,PC_NODE_ADDR,1);
			break;
		case 'W':
			//写入操作
			DISABLE_INTERRUPTS;//关中断
			//帧序列号
			frameOrder = flash_write_temp[1];
			//写入长度
			write_len = flash_write_temp[2];
			//根据frameOrder计算对应的扇区号和偏移量
			getCurrentSectorAndOffset(frameOrder,&sector,&offset);
			//写入一个frame
			flash_write(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_write_temp+3);
			//打印调试信息
//			uart_send1(UART_0, frameOrder/10+'0');
//			uart_send1(UART_0, frameOrder%10+'0');
//			uart_send_string(UART_0,"frame\r\n");
			ENABLE_INTERRUPTS;//开中断
			break;
		case 'S':
			//写入结束
			sector = 0;
			offset = 0;
			break;
		case 'M':
			//重新读取丢失的frame
			DISABLE_INTERRUPTS;//关中断
			//打印调试信息
//			uart_send1(UART_0, flash_write_temp[1]/10+'0');
//			uart_send1(UART_0, flash_write_temp[1]%10+'0');
//			uart_send_string(UART_0," frames pc feel miss \r\n");

			//flash_write_temp[0]:'M'
			//flash_write_temp[1]:丢失的个数
			//flash_write_temp[2~2+丢失的个数]:丢失的frameOrder
			//将每个丢失的frame重新发送
			for(i=0;i<flash_write_temp[1];i++){
				frameOrder = flash_write_temp[2+i];
				getCurrentSectorAndOffset(frameOrder,&sector,&offset); //根据frameOrder计算对应的扇区号和偏移量
				flash_read(FLASH_START_SECTOR+sector,offset*MaxFrameLength,MaxFrameLength,flash_read_temp);
				WPSENDLargeDataWithFrame(flash_read_temp,MaxFrameLength,PC_NODE_ADDR,frameOrder);
			}
			//发送结束信息
			WPSendData("a", 1, NZP_TS_END, PC_NODE_ADDR, 0);
			ENABLE_INTERRUPTS;//开中断
			break;

		default:
			break;
		}

	}//任务循环体end_while
}

