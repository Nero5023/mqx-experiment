//===========================================================================
//文件名称：task_light.c
//功能概要：控制运行指示灯闪烁（引脚：PTD7）
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"    //应用任务公共头文件

// 宏定义任务内部使用的常量
#define ONE_SECOND_DELAY  200

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
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		//1）控制运行指示灯闪烁
		light_change(LIGHT_RUN_1);      //改变运行指示灯引脚电平状态(引脚：PTD7)

		//2）任务延时1秒
		_time_delay_ticks(1*ONE_SECOND_DELAY); //每个tick对应5ms，延时200*5ms=1s
		WPSendData("Hello?",6,NZP_DATA,15,0);

	}//任务循环体end_while
}
