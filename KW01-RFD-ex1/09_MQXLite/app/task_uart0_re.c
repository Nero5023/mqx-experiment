//===========================================================================
//文件名称：task_uart0_re.c
//功能概要：放置Uart0接收数据帧的解帧任务
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"    //应用任务公共头文件

extern uint_8 HW_ADR;
//===========================================================================
//任务名称：task_uart0_re
//功能概要：判断串行接收事件位Event_Uart0_Re，解析串行接收数据帧，根据数据帧内容决定：1会送握手信号，
//       2直接串口会送接收信息，3设置RF发送轻量级事件位Event_RF_Send，启动RF发送任务task_rf_send()。
//参数说明：未使用
//===========================================================================
void task_uart0_re(uint32_t initial)
{	
	//1. 声明任务使用的变量
	uint_16 i;
	uint_16 offset;
	uint_16 count;
	uint_16 ret;	//操作返回值
	//2. 给有关变量赋初值

	//3. 进入任务循环体
	while(TRUE) 
	{
		//以下加入用户程序--------------------------------------------------------
		//1）无限等待“串口0接收完整数据帧事件位”
		_lwevent_wait_for(&lwevent_group, EVENT_UART0_RE, FALSE, NULL);

		DISABLE_INTERRUPTS;
		// 根据硬件过滤地址，判断帧类型（握手帧还是转发帧）
		uart_send_string(UART_0,"Got a frame.");


		ENABLE_INTERRUPTS;

        //3）Uart0接收事件位清零
		_lwevent_clear(&lwevent_group, EVENT_UART0_RE);
	}//任务循环体end_while
}
