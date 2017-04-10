#include "01_app_include.h"
//===========================================================================
//任务名称：task_wp_register
//功能概要：根据自定的无线协议在网络中注册自己
//===========================================================================


#define ONE_SECOND_DELAY 200
#define REGISTER_TIMEOUT 3

void task_wp_register(uint32_t initial){
	generateRegisterData();		//生成注册信息
	while(TRUE){
		uint_8 wait_time=0;
		switch (net_status) {
			case UNREGISTERED: //尚未注册则开始注册

				HD_adr=255;
				RF_Init(HD_adr);		    //RF模块初始化
				_lwevent_set(&lwevent_group,EVENT_RF_SEND);	//置RF发送事件位（EVENT_RF_SEND），触发task_rf_send任务
				net_status = REGISTERING;//标记状态为注册中

				break;

			case REGISTERING: //注册中,等待主机返回结果
				//延时1秒
				_time_delay_ticks(1*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s
				wait_time++;
				if(wait_time >= REGISTER_TIMEOUT){
					net_status = UNREGISTERED; //注册超时
					wait_time = 0;
				}

			case REGISTERING_WITH_ECHO:
				//判断是否注册成功

				break;
			case REGISTERED:
				//注册成功,函数结束
				return;
			default:
				break;
		}
	}
}
