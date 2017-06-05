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
		_mqx_uint recv_msg[RECV_MSG_SIZE];//用于接收recv_queue中的消息

		_lwmsgq_receive((pointer)recv_queue,recv_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		uint_8 length = length_of_NZP((pointer)recv_msg);
		uint_8 data_length = data_length_of_NZP((pointer)recv_msg);


		char data[56];
		// 解析 NZP 协议，如果解析成功（发送给自己的，checksum 正确）
		if (parse_NZP((pointer)recv_msg, length, data)) {
			// uart data
			// 获取 NZP 的类型
			NZP_TYPE type = type_of_NZP((pointer)recv_msg);
			// 获取发送源的地址
			uint_8 addr = addr_of_NZP((pointer)recv_msg);

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
					receive_heartbeat_from(addr);
					break;
				case NZP_HEARTBEAT:
					receive_heartbeat_from(addr);
					break;
				case NZP_RTS:
					// CAN_NOT_SEND mean there is no other things
					// if Lage_Data_Flag == CAN_NOT_SEND: {
					// 	Lage_Data_Flag = IS_SENDING;
					// 	WPSendData("1", 1, NZP_CTS, addr, 0);
					// }else {
					// 	WPSendData("0", 1, NZP_CTS, addr, 0);
					// }
					// Lage_Data_Flag = IS_SENDING;
					sendBigDataStart(addr, data[0]);
					break;
				case NZP_CTS:
					// if (data[0] == "1") {
					// 	Lage_Data_Flag = CAN_SEND;
					// } else {
					// 	Lage_Data_Flag = CAN_NOT_SEND;
					// }
					break;
				case NZP_TS_DATA:
					// uint_8 frameOrder = data[0];
					// data += 1;
					sendBigData(data, data_length);
					break;
				case NZP_TS_END:
					// Lage_Data_Flag = CAN_NOT_SEND;
					uart_send_string(UART_0,"Send ACK");
					sendBigDataEnd();//uart send to pc
					//WPSendData("a", 1, NZP_ACK, addr, 0);
					break;
				case NZP_ACK:
					//data[0]: the number of missed frame
					if(data[0]==0){
						break;
					}
					else{
						sendBigDataMiss(data);
					}
					break;
				default:
					break;
			}


		}
		
	}//任务循环体end_while
}

