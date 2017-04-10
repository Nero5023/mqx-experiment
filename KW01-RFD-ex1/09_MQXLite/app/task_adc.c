/*
 * task_adc.c
 *
 *  Created on: 2016年3月24日
 *      Author: asus
 */
#include "01_app_include.h"    //应用任务公共头文件

//===========================================================================
//任务名称：task_adc
//功能概要：读取26通道的温度物理量，计算出温度值，
//                该任务优先级为14
//参数说明：未使用
//===========================================================================
void task_adc(uint32_t initial_data)
{
	//1. 声明任务使用的变量
	uint_16 temp;        //用于存储温度物理量的读取
	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE)
	{
		//以下加入用户程序--------------------------------------------------------
		//1）获取26通道的温度物理量
		temp=adc_read(26);
		      //物理量回归，得到摄氏温度值
		g_temperature=(25.0-(temp*3.3*1000/1024-719)/1.715);
		//2）任务延时1秒
		_time_delay_ticks(5*200); //延时200*5ms=1s
	}
	//任务循环体end_while
}




