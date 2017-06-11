#include "01_app_include.h"    //应用任务公共头文件


typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't',         // data[1] 代表 发送的目的地
    PC_COM_CONTIONUOUS = 'c',  // data[1] 代表 发送的目的地
    PC_BIG_DATA_START = 'l', // l | totoalLength | destination
    PC_BIG_DATA = 'b',       // d | dataLength | data
    PC_BIG_DATA_END = 'e',    // e |
    PC_BIG_DATA_READ = 'r',  // r | address
	PC_MISS_DATA     = 'M',  // M | addr |miss data length | data
	PC_LIGHT_CONTROL_MESSAGE = 'L' // L  | addr | value
} PC_COM_TYPE;

void task_pc_command(uint32_t initial_data){
    // 消息队列数据
    _mqx_uint pc_com_msg[COMMAND_MSG_SIZE];
    uint_8 count = 0;
    uint_8 i = 0;

    uint_8 totoalLength = 0;
    uint_8 destination = 0;  // destination addr
    uint_8 dataLength;
    char des;
    while (TRUE) {
        // 等待 pc_com_msg 消息队列（等待 pc_command ）
        _lwmsgq_receive((pointer)pccommand_queue,pc_com_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); 
        char *data = pc_com_msg;
        uint_8 aliveAddrs[REG_COUNT];
        uint_8 aliveCount;
        uint_8 j = 0;
        uint_8 frameOrder;
        switch (data[0]) {
            case PC_COM_NODES:        // 询问所有的节点状态
                aliveCount = alive_node_addrs(aliveAddrs); // 获取所有存活的节点地址
                sendNodeStatus(aliveCount,aliveAddrs);     // 发送 UART 给 PC 看看
                break;
            case PC_COM_TEMP:        // 询问节点温度
                des = data[1];       // 询问地址为 des 节点的温度信息
                WPSendData("a", 1, NZP_TEMPERATURE, des-'0', 0); // 发送询问协议
                break;
            case PC_COM_CONTIONUOUS: // 持续监测温度信息
                des = data[1];
                WPSendData("a", 1, NZP_CONTIONUOUS_MONITOR, des-'0', 0);
                break;
            case PC_BIG_DATA_START:  // 大数据的头帧
                totoalLength = data[1];  // 总的帧数
                destination = data[2];   // 目的地址
                WPSendData(&totoalLength, 1, NZP_RTS, destination, 0);
                break;
            case PC_BIG_DATA:       // 大数据的每一帧
            	frameOrder = data[1];  // 帧号
                dataLength = data[2];  // 数据长度
                WPSENDLargeDataWithFrame(data+3, dataLength, destination, frameOrder);
                break;
            case PC_BIG_DATA_END:   // 数据尾
            	WPSendData("a", 1, NZP_TS_END, destination, 0);
                break;
            case PC_BIG_DATA_READ:  // 读取节点的 flash 的数据
            	WPSendData("a",1,NZP_DATA_READ,data[1],0);
            	break;
            case PC_MISS_DATA:  // 发送丢失数据帧
            	// data[2]: the number of miss frames
            	destination = data[1]; // 目标节点
            	data[1] = 'M';
            	WPSendData(data+1, data[2]+2, NZP_ACK, destination, 0);
            	break;
            case PC_LIGHT_CONTROL_MESSAGE:  // 控制小灯频率控制
            	destination = data[1];      // 目标节点地址 
            	WPSendData(data+2, 1, NZP_LIGHT_CONTROL, destination, 0);
            	break;
            default:
                break;
        }

    }
}

