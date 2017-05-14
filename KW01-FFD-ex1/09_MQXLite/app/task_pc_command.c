#include "01_app_include.h"    //应用任务公共头文件

#define PC_COM_MSG_SIZE 16

typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't',  // data[1] 代表 发送的目的地
    PC_COM_CONTIONUOUS = 'c' // data[1] 代表 发送的目的地
} PC_COM_TYPE;

void task_pc_command(uint32_t initial_data){
    // 消息队列数据
    _mqx_uint pc_com_msg[PC_COM_MSG_SIZE];
    uint_8 count = 0;
    uint_8 i = 0;
    char des;
    while (TRUE) {
        // 等待 pc_com_msg 消息队列（等待 pc_command ）
        _lwmsgq_receive((pointer)pccommand_queue,pc_com_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); 
        char *data = pc_com_msg;
        uint_8 aliveAddrs[REG_COUNT];
        uint_8 aliveCount;
        uint_8 j = 0;
        switch (data[0]) {
            case PC_COM_NODES:        // 询问所有的节点状态
                aliveCount = alive_node_addrs(aliveAddrs);
                sendNodeStatus(aliveCount,aliveAddrs);
                break;
            case PC_COM_TEMP:
                des = data[1];       // 询问地址为 des 节点的温度信息
                WPSendData("a", 1, NZP_TEMPERATURE, des-'0', 0);
                break;
            case PC_COM_CONTIONUOUS:
                des = data[1];
                WPSendData("a", 1, NZP_CONTIONUOUS_MONITOR, des-'0', 0);
                break;
            default:
                break;
        }

    }
}

