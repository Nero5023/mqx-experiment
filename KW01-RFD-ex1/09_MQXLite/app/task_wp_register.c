#include "01_app_include.h"
//===========================================================================
//任务名称：task_wp_register
//功能概要：根据自定的无线协议在网络中注册自己
//===========================================================================


#define ONE_SECOND_DELAY 200

void task_wp_register(uint32_t initial){
	generateRegisterData();		//生成注册信息
	while(TRUE){
		//任务延时3秒
		_time_delay_ticks(3*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s

		if(net_status==UNREGISTERED){
			HD_adr=255;
		    RF_Init(HD_adr);		    //RF模块初始化
			_lwevent_set(&lwevent_group,EVENT_RF_SEND);	//置RF发送事件位（EVENT_RF_SEND），触发task_rf_send任务
		}else{
			return;
		}
	}
}
