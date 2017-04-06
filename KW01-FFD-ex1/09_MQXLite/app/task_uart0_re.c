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
		uart_send_string(UART_0,"Got a frame from UART0.\n");
		if(g_uart_recvBuf[2] == 0)    //握手命令
	    	{
	    		g_uart_sentBuf[0]=(uint_8)'M';     //
	    		g_uart_sentBuf[1]=7;               //
	    		g_uart_sentBuf[2]  = (uint_8)'C';  //
	    		g_uart_sentBuf[3]  = (uint_8)'P';  //"PCNode"
	    		g_uart_sentBuf[4]  = (uint_8)'C';
	    		g_uart_sentBuf[5]  = (uint_8)'N';
	    		g_uart_sentBuf[6]  = (uint_8)'o';
	    		g_uart_sentBuf[7]  = (uint_8)'d';
	    		g_uart_sentBuf[8]  = (uint_8)'e';
	    		g_uart_sentBuf[9]  = (uint_8)'U';  //
				uart_sendN(UART_0,10,&g_uart_sentBuf[0]);
	    	}
		else      // 启动转发
		{
			HD_adr=g_uart_recvBuf[2];   //HD_adr为全局变量，硬件地址
		    RF_Init(HD_adr);		    //RF模块初始化
			 _lwevent_set(&lwevent_group,EVENT_RF_SEND);	//置RF发送事件位（EVENT_RF_SEND），触发task_rf_send任务
		}
//		//2）解帧g_uart_recvBuf,g_uart_recvBuf[2]是命令字节
//		switch(g_uart_recvBuf[2])
//		{
//		case 'A':    //直接发送测试数据包
//			g_uart_recvBuf[2]=60;
//			uart_sendN(UART_0,60,&g_uart_recvBuf[3]);
//			break;
//		case 'B':    //RF转发测试数据包
//		    _lwevent_set(&lwevent_group,EVENT_RF_SEND);	//置RF发送事件位（EVENT_RF_SEND），触发task_rf_send任务
//			break;
//			//握手处理改在ISR中处理
//		case 'D':
//			_lwevent_set(&lwevent_group,EVENT_FLASH);	//置Flash事件位，启动Flash处理任务
//			break;
//		case 'E':
//			HD_adr=g_uart_recvBuf[9];   //HD_adr为全局变量，硬件地址
//		    RF_Init(HD_adr);		         //RF模块初始化
//			uart_sendN(UART_0,1,&g_uart_recvBuf[9]);
//			break;
//		default:
//			break;
//		}

		ENABLE_INTERRUPTS;

        //3）Uart0接收事件位清零
		_lwevent_clear(&lwevent_group, EVENT_UART0_RE);
	}//任务循环体end_while
}
