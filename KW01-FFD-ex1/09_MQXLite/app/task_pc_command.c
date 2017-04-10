#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

#define PC_COM_MSG_SIZE 16

typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't'  // data[1] ���� ���͵�Ŀ�ĵ�
} PC_COM_TYPE;

void task_pc_command(uint32_t initial_data){
    _mqx_uint pc_com_msg[PC_COM_MSG_SIZE];
    uint_8 count = 0;
    int i = 0;
    while (TRUE) {
        _lwmsgq_receive((pointer)pccommand_queue,pc_com_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); //�ȴ�pc_command
        char *data = pc_com_msg;
        switch (data[0]) {
            case PC_COM_NODES:        // all nodes of 
                while (rfdAddrs[count] != 0) { count++; }

                char str[64] = "Have x nodes "
                str[5] = count + '0';
                for (i = 13; i < 13+count*2; i+=2) {
                    str[i] = rfdAddrs[i] + '0';
                    str[i+1] = ' ';
                }

                uart_sendN(UART_0, 13+count*2, str);

                break;
            case PC_COM_TEMP:
                char des = data[1];
                WPSendData("a", 1, NZP_REGISTER_Success, 0xff, 0);
                break;
            default:
                break;
        }

    }
}

