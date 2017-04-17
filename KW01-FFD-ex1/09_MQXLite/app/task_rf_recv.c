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

		uint_8 length = length_of_NZP((pointer)rf_recvBuf);
		uint_8 data_length = data_length_of_NZP((pointer)rf_recvBuf);


		char data[56];
		// 解析 NZP 协议，如果解析成功（发送给自己的，checksum 正确）
		if (parse_NZP((pointer)rf_recvBuf, length, data)) {
			// uart data
			// 获取 NZP 的类型
			NZP_TYPE type = type_of_NZP((pointer)rf_recvBuf);
			// 获取发送源的地址
			uint_8 addr = addr_of_NZP((pointer)rf_recvBuf);

			switch (type) {
				case NZP_REGISTER:// 有注册消息到来
					_lwmsgq_send((pointer)register_queue,data,LWMSGQ_SEND_BLOCK_ON_FULL); //放入注册消息队列中
					break;
				case NZP_DATA:// 需要直接打印的消息
					uart_sendN(UART_0, data_length, data);
					break;
				case NZP_TEMPERATURE: // 收到温度信息时候
					sendNodeTempInfo(addr,data);
					break;
				case NZP_REGISTER_Success: // 收到注册成功消息
					sendNodeReisterSuccessInfo(addr);
					break;
				default:
					break;
			}


		}
		
		//3）RF接收事件位清零
		_lwevent_clear(&lwevent_group, EVENT_RF_RECV);
	}//任务循环体end_while
}

