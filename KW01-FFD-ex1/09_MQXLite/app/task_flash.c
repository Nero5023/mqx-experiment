//===========================================================================
//文件名称：task_flash.c
//功能概要：FLASH操作(读、写、擦除)
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================

#include "01_app_include.h"    //应用任务公共头文件
uint_8  bufRetErr[18]  = {'M',0x15,'F','l','a','s','h','-','x','x','x','x','x','-','E','r','r'};
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
	int offset,count;
	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		int i;
		uint_8 sector;

		//1）无限等待RF接收事件位置一
		_lwevent_wait_for(&lwevent_group, EVENT_FLASH, FALSE, NULL);

		//2）调用接收函数
		//读取flash指定扇区操作
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
			DISABLE_INTERRUPTS;//关中断
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
			ENABLE_INTERRUPTS;//开中断
		}

		//3）清除事件位
		_lwevent_clear(&lwevent_group, EVENT_FLASH);
	}//任务循环体end_while
}

