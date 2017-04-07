//============================================================================
//文件名称：wp.h
//功能概要：无线通信及组网协议模块
//============================================================================

#ifndef WP_H
#define WP_H


typedef enum NET_STATUS{
	UNREGISTERED=1, //尚未注册
	REGISTERING, //注册中
	REGISTERED //已注册
}NET_STATUS;

static NET_STATUS net_status = UNREGISTERED;


void generateRegisterData();

#endif
