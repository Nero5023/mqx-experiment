//===========================================================================
//文件名称：task_rf_send.c
//功能概要：放置RF发送数据帧任务
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"    //应用任务公共头文件

//===========================================================================
//任务名称：task_rf_send
//功能概要：判断RF发送事件位Event_RF_Send，调用RF_SendData()函数转发接收测试数据包；
//参数说明：未使用
//===========================================================================
void task_rf_send(uint32_t initial)
{	
	//1. 声明任务使用的变量
	uint_8 i;


	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		//1）无限等待RF发送事件位置一
		_lwevent_wait_for(&lwevent_group, EVENT_RF_SEND, FALSE, NULL);

		/*
		rf_sentDataLen = g_uart_recvBuf[1]-1;  //需要RF转发的字节数

		for (i=0;i<rf_sentDataLen;i++) rf_sentBuf[i]=g_uart_recvBuf[i+3];


		//2）RF模块发送数据,第三个参数，不检查信道和能量值
		if(g_uart_recvBuf[58]=='N')
	        RFSendDataByCSMACA(rf_sentDataLen,&rf_sentBuf[0],0,HD_adr);
		else
	        RFSendDataByCSMACA(rf_sentDataLen,&rf_sentBuf[0],1,HD_adr);
		*/
//		RFSendDataByCSMACA(15,"An RF test msg.",0,HD_adr);
//		uart_send_string(UART_0,"Sended a test frame.\n");

		RFSendDataByCSMACA(rf_sentDataLength,rf_sentBuf,0,255);

		//清除RF发送事件位
		_lwevent_clear(&lwevent_group, EVENT_RF_SEND);

	}//任务循环体end_while
}
