#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

char generateAddr() {
    int i = 0;
    while (rfdAddrs[i] != 0) {
        i++;
    }
    rfdAddrs[i] = (i+1);
    return i+1;
}

void removeAddr(char addr) {
    if (addr < 8)
        rfdAddrs[addr] = 0;
}


void task_register_process(uint32_t initial_data){

	_mqx_uint re_msg[RE_MSG_SIZE];

	while(TRUE){
		_lwmsgq_receive((pointer)register_queue,re_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); //�ȴ�ע����Ϣ

		uart_sendN(UART_0,9,"Receive re_msg.");
        char *data = re_msg;

        char addr = generateAddr();
        rfdKeys[addr] = data[0];  //��Ŷ�Ӧ�ڵ����Կ
        char dataToSend[2] = {data[0], addr};
        uart_sendN(UART_0,2,dataToSend);
        encode(dataToSend, dataToSend, 2, data[0]);
//        uart_sendN(UART_0,2,dataToSend);
        WPSendData(dataToSend, 2, NZP_REGISTER_Success, 0xff, 0);
//
//		uart_sendN(UART_0,RE_MSG_SIZE*4,data);

	}
}
