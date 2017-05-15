//===========================================================================
//文件名称：task_flash.c
//功能概要：FLASH操作(读、写、擦除)
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================

#include "01_app_include.h"    //应用任务公共头文件
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

	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		int i;

		//1）无限等待RF接收消息
		_lwmsgq_receive((pointer)flash_write_queue,flash_write_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		//2）调用接收函数
		//读取flash指定扇区操作
		switch(flash_write_temp[0]){

		case 'B':
			//开始写入flash,记录下总帧数
			total_len = flash_write_temp[1];
			flash_erase(FILE_INFO_SECTOR);
			flash_write(FILE_INFO_SECTOR,0,1,total_len);
			break;

		case 'R':
			flash_read(FILE_INFO_SECTOR,0,1,&total_len); //从flash读出文件长度
			i=1;
			while(i<total_len){
				getCurrentSectorAndOffset(frameOrder,&sector,&offset); //根据frameOrder计算对应的扇区号和偏移量
				flash_read(FLASH_START_SECTOR+sector,offset*52,MaxFrameLength,flash_write_temp);
				WPSENDLargeData(flash_write_temp,MaxFrameLength,total_len,PC_NODE_ADDR,0);
				i++;
			}
			WPSENDLargeData("",MaxFrameLength,total_len,PC_NODE_ADDR,1); // 发送结束帧

			break;

		case 'W':
			DISABLE_INTERRUPTS;//关中断

			frameOrder = flash_write_temp[1];

			getCurrentSectorAndOffset(frameOrder,&sector,&offset); //根据frameOrder计算对应的扇区号和偏移量

			flash_write(FLASH_START_SECTOR+sector,offset*52,MaxFrameLength,((char*)flash_write_temp)+1);


			ENABLE_INTERRUPTS;//开中断
			break;
		case 'S':
			//写入结束
			sector = FLASH_START_SECTOR;
			offset = 0;
		case 'E':
			DISABLE_INTERRUPTS;//关中断
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
			ENABLE_INTERRUPTS;//开中断
		}

	}//任务循环体end_while
}

