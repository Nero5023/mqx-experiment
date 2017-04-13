#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

#define PC_COM_MSG_SIZE 16

typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't'  // data[1] ���� ���͵�Ŀ�ĵ�
} PC_COM_TYPE;

void task_pc_command(uint32_t initial_data){
    _mqx_uint pc_com_msg[PC_COM_MSG_SIZE];
    uint_8 count = 0;
    uint_8 i = 0;
    char des;
    while (TRUE) {
        _lwmsgq_receive((pointer)pccommand_queue,pc_com_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); //�ȴ�pc_command
        char *data = pc_com_msg;
        uint_8 j = 0;
        switch (data[0]) {
            case PC_COM_NODES:        // all nodes of 
                while (rfdAddrs[count] != 0) { count++; }
                sendNodeStatus(count,rfdAddrs);
                break;
            case PC_COM_TEMP:
                des = data[1];
                WPSendData("a", 1, NZP_TEMPERATURE, des-'0', 0);
                break;
            default:
                break;
        }

    }
}

