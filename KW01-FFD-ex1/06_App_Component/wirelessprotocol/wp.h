//============================================================================
//文件名称：wp.h
//功能概要：无线通信及组网协议模块
//============================================================================

#ifndef WP_H
#define WP_H
#include "Protocol.h"

#define MaxFrameLength 48

typedef enum NET_STATUS{
	UNREGISTERED=1, 			//尚未注册 		--> task_wp_register函数发送注册信息
	REGISTERING, 				//注册中  			--> task_rf_recv函数等待接收返回
	REGISTERING_WITH_ECHO,		//注册中且收到反馈 	--> task_wp_register函数判断注册是否成功
	REGISTERED 					//已注册 			--> task_wp_register函数终止
}NET_STATUS;

NET_STATUS net_status;


void generateRegisterData();

//=====================================================================
//函数名称：WPSendData
//函数返回：无返回值
//参数说明：   data: 将要发送数据的字符串指针
//         length: 发送的数据长度
//       NZP_TYPE: 发送 NZP 协议的种类
//    destination: 发送的目的地
//  needEncrypted: 是否需要发送目的地
//功能概要：发送一帧 rf 数据
//=====================================================================
void WPSendData(char *data, char length, enum NZP_TYPE type, char destination, char needEncrypted);


//=====================================================================
//函数名称：WPSENDLargeData
//函数返回：0 表示发送失败
//        1 表示发送成功
//参数说明：   data: 将要发送数据的字符串指针
//         length: 发送的数据长度
//    totalLength: 发送的总数据长度, 总数据帧数
//    destination: 发送的目的地
//            end: 是否结束发送
//功能概要：发送一帧 rf 大数据
//=====================================================================
uint_8 WPSENDLargeData(uint_8 *data, uint_8 length, uint_8 totalLength,char destination, uint_8 end);

//=====================================================================
//函数名称：WPSENDLargeData
//函数返回：无返回值
//参数说明：   data: 将要发送数据的字符串指针
//         length: 发送的数据长度
//    destination: 发送的目的地
//          count: 数据 frame order
//功能概要：发送一帧 rf 大数据
//=====================================================================
void WPSENDLargeDataWithFrame(uint_8 *data, uint_8 length, char destination, uint_8 count);

#endif
