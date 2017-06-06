//===========================================================================
//文件名称：task_light.c
//功能概要：控制运行指示灯闪烁（引脚：PTD7）
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"    //应用任务公共头文件

// 宏定义任务内部使用的常量
#define ONE_SECOND_DELAY  200



void SendHeartBeart(){
	if(net_status==REGISTERED){
		WPSendData("a",1,NZP_HEARTBEAT,PC_NODE_ADDR,0);
	}
}




//===========================================================================
//任务名称：task_light
//功能概要：控制运行指示灯每隔1s闪烁，该任务优先级为14，仅仅高于task_adc
//参数说明：未使用
//===========================================================================


void task_light(uint_32 initial)
{	
	//const uint32_t X1=0x01020304;
	//const uint32_t X1=0x01020304 @0x30004000;
	// 进入任务循环体
	uint_8 i = 0;
	uint_8 modNum = 0;
	while(TRUE) 
	{
////		uart_send1(UART_0,'a');
//		//以下加入用户程序--------------------------------------------------------
////		//1）控制运行指示灯闪烁
//		light_change(LIGHT_RUN_1);      //改变运行指示灯引脚电平状态(引脚：PTD7)
//		//2）任务延时1秒
//		_time_delay_ticks(0.5*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s
//		SendHeartBeart();
//		_time_delay_ticks(0.5*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s
//
//		SendHeartBeart();

		i = light_control_value/25 + 1;
		if(i == 1)
			modNum = 8;
		if(i == 2)
			modNum = 4;
		if (i == 3)
			modNum = 2;
		if (i == 4)
			modNum = 1;
		if (light_control_value == 0)
			light_control(LIGHT_RUN_1, LIGHT_OFF);
		if (light_control_value == 100)
			light_control(LIGHT_RUN_1, LIGHT_ON);
		for (i = 0; i < 8; i++) {
			_time_delay_ticks(ONE_SECOND_DELAY/8);
			if (i == 0 || i == 3) {
				SendHeartBeart();
			}
			if (light_control_value == 0 || light_control_value == 100) {
				continue;
			}
			if (i%modNum == 0) {
				light_change(LIGHT_RUN_1);
			}
		}
	}//任务循环体end_while
}



