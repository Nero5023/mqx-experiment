//===========================================================================
//文件名称：task_rf_recv.c
//功能概要：放置RF接收数据帧任务
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================

#include "01_app_include.h"    //应用任务公共头文件

//===========================================================================
//任务名称：task_rf_recv
//功能概要：判断RF接收事件位EVENT_RF_RECV，调用RF_ReceiveFrame()接收PC Node节点RF测试数据包，
//       解析成功后采样本节点芯片温度以及RF能量值并送入待发送数据的最后三个字节中，设置RF发送轻量级事件位EVENT_RF_SEND，
//       启动RF发送任务task_rf_send()。
//参数说明：未使用
//===========================================================================
void task_rf_recv(uint32_t initial)
{	
	//1. 声明任务使用的变量
	uint_8 i;

	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		//1）无限等待RF接收事件位置一
		_lwevent_wait_for(&lwevent_group, EVENT_RF_RECV, FALSE, NULL);
//		uart_send_string(UART_0,"Got a msg from RF.\n");
		// uart_sendN(UART_0,64,rf_recvBuf);
		//串口发送数据全局数组
//		g_uart_sentBuf[0]='M';    //77
//
//		g_uart_sentBuf[1]=g_rfRecCount+1;
//
//		g_uart_sentBuf[2]='B';                    //66
//
//		for (i=3;i<=g_uart_sentBuf[1]+1;i++)	g_uart_sentBuf[i]=rf_recvBuf[i-3];
//
//		g_uart_sentBuf[g_uart_sentBuf[1]+2]='U';   //85=U

		//2）调用接收函数
		//rf_recvBuf[60]=(uint_16)g_temperature;   		     //整数部分
		//rf_recvBuf[61]=(g_temperature-rf_recvBuf[60])*10+0.5;//小数部分(保留一位小数，四舍五入)
		//rf_recvBuf[62]=RFEnergyDetect();

//uart_sendN(UART_0,g_uart_sentBuf[1]+3,&g_uart_sentBuf[0]);              //通过测试串口0发送数据

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
		
		//3）RF接收事件位清零
		_lwevent_clear(&lwevent_group, EVENT_RF_RECV);
	}//任务循环体end_while
}

