#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

void task_register_process(uint32_t initial_data){

	_mqx_uint re_msg[RE_MSG_SIZE];

	while(TRUE){
		_lwmsgq_receive((pointer)register_queue,re_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); //�ȴ�ע����Ϣ
		uart_sendN(UART_0,RE_MSG_SIZE*4,re_msg);

	}
}
