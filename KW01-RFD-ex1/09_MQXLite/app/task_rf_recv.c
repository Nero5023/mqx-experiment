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
		_mqx_uint recv_msg[RECV_MSG_SIZE];//用于接收recv_queue中的消息

		//以下加入用户程序--------------------------------------------------------
		//1）无限等待RF接收事件位置一
		_lwmsgq_receive((pointer)recv_queue,recv_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0);

		uint_8 length = length_of_NZP(recv_msg);
		uint_8 data_length = data_length_of_NZP(recv_msg);
		char data[56];

		// 解析 NZP 协议，如果解析成功（发送给自己的，checksum 正确）
		if (parse_NZP(recv_msg, length, data)) {

			// 用于 debug
			uart_sendN(UART_0,data_length,data);

			// 获取数据报协议类型
			NZP_TYPE type = type_of_NZP(recv_msg);

			char clearText[2];
			switch (type) {
				case NZP_REGISTER_Success:       // 接收到注册回复信息
					// 解密数据

					decode(data, clearText, 2, ENCRYPT_KEY);
					if (clearText[0] == ENCRYPT_KEY && net_status == REGISTERING) { // 解密信息中的密钥与自己密钥相同时，即为发送给自己的
						SELF_ADDR = clearText[1];  // 设置自己的地址
						net_status = REGISTERING_WITH_ECHO;  // 将注册状态置为 REGISTERING_WITH_ECHO
					}
					break;
				case NZP_TEMPERATURE:
					// 发送对应的全局变量
					WPSendData(&g_temperature,4,NZP_TEMPERATURE,PC_NODE_ADDR,0);
					break;
				case NZP_CONTIONUOUS_MONITOR:
					if(is_continous_monitoring==0){
						is_continous_monitoring=1;
					}else{
						is_continous_monitoring=0;
					}
					break;
				case NZP_RTS:
					// CAN_NOT_SEND mean there is no other things
					if Lage_Data_Flag == CAN_NOT_SEND: {
						Lage_Data_Flag = IS_SENDING;
						WPSendData("1", 1, NZP_CTS, addr, 0);
					}else {
						WPSendData("0", 1, NZP_CTS, addr, 0);
					}
					break;
				case NZP_CTS:
					if (data[0] == "1") {
						Lage_Data_Flag = CAN_SEND;
					} else {
						Lage_Data_Flag = CAN_NOT_SEND;
					}
					break;
				case NZP_TS_DATA:
					uint_8 frameOrder = data[0];
					data += 1;
					
					break;
				case NZP_TS_END:
					Lage_Data_Flag = CAN_NOT_SEND;
					WPSendData("a", 1, NZP_ACK, destination, 0);
					break;
				case NZP_ACK:
					break;
				default:
					break;
			}
		}
		else{
			// 用于 Debug
			uart_send_string(UART_0,"Parse Failed.");
		}

	}//任务循环体end_while
}

