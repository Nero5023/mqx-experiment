#include "01_app_include.h"    //应用任务公共头文件

void task_register_process(uint32_t initial_data){

	_mqx_uint re_msg[RE_MSG_SIZE];

	while(TRUE){
		_lwmsgq_receive((pointer)register_queue,re_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); //等待注册消息
		uart_sendN(UART_0,RE_MSG_SIZE*4,re_msg);

	}
}
