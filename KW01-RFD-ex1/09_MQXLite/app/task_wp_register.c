#include "01_app_include.h"
//===========================================================================
//任务名称：task_wp_register
//功能概要：根据自定的无线协议在网络中注册自己
//===========================================================================


#define ONE_SECOND_DELAY 200
#define REGISTER_TIMEOUT 3

void task_wp_register(uint32_t initial){
	net_status = UNREGISTERED;
	uint_8 wait_time=0;
	while(TRUE){
		// uint_8 wait_time=0;
		switch (net_status) {
			case UNREGISTERED: //尚未注册则开始注册
				if (SELF_ADDR == 0) {
					srand(0);
				    char key[4] = {rand() % 100};
				    key[1] = 'a';
				    key[2] = 'b';
				    key[3] = 'c';
				    ENCRYPT_KEY = key[0];
					WPSendData(key, 4, NZP_REGISTER, 0xff, 0);
					net_status = REGISTERING;//标记状态为注册中
				}
				break;

			case REGISTERING: //注册中,等待主机返回结果
				//延时1秒
				_time_delay_ticks(1*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s
				wait_time++;
				if(wait_time >= REGISTER_TIMEOUT){
					net_status = UNREGISTERED; //注册超时
					wait_time = 0;
				}
				break;
			case REGISTERING_WITH_ECHO:
			// 	//判断是否注册成功
				net_status = REGISTERED;
				break;
			case REGISTERED:
				//注册成功,函数结束
				return;
			default:
				break;
		}
	}
}
