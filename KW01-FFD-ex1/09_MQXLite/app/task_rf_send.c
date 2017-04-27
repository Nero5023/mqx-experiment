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

		_mqx_uint send_msg_temp[SEND_MSG_SIZE];
		_lwmsgq_receive((pointer)send_queue,send_msg_temp,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);
		uint_8 len =  length_of_NZP(send_msg_temp);
		RFSendDataByCSMACA(len,send_msg_temp,0,255);

	}//任务循环体end_while
}
