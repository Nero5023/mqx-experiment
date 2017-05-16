
#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�


typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't',         // data[1] ���� ���͵�Ŀ�ĵ�
    PC_COM_CONTIONUOUS = 'c',  // data[1] ���� ���͵�Ŀ�ĵ�
    PC_BIG_DATA_START = 'l', // l | totoalLength | destination
    PC_BIG_DATA = 'b',       // d | dataLength | data
    PC_BIG_DATA_END = 'e',    // e |
    PC_BIG_DATA_READ = 'r'  // r | address
} PC_COM_TYPE;

void task_pc_command(uint32_t initial_data){
    // ��Ϣ��������
    _mqx_uint pc_com_msg[COMMAND_MSG_SIZE];
    uint_8 count = 0;
    uint_8 i = 0;

    uint_8 totoalLength = 0;
    uint_8 destination = 0;  // destination addr
    uint_8 dataLength;
    char des;
    while (TRUE) {
        // �ȴ� pc_com_msg ��Ϣ���У��ȴ� pc_command ��
        _lwmsgq_receive((pointer)pccommand_queue,pc_com_msg,LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,0,0); 
        char *data = pc_com_msg;
        uint_8 aliveAddrs[REG_COUNT];
        uint_8 aliveCount;
        uint_8 j = 0;
        switch (data[0]) {
            case PC_COM_NODES:        // ѯ�����еĽڵ�״̬
                aliveCount = alive_node_addrs(aliveAddrs);
                sendNodeStatus(aliveCount,aliveAddrs);
                break;
            case PC_COM_TEMP:
                des = data[1];       // ѯ�ʵ�ַΪ des �ڵ���¶���Ϣ
                WPSendData("a", 1, NZP_TEMPERATURE, des-'0', 0);
                break;
            case PC_COM_CONTIONUOUS:
                des = data[1];
                WPSendData("a", 1, NZP_CONTIONUOUS_MONITOR, des-'0', 0);
                break;
            case PC_BIG_DATA_START:
                totoalLength = data[1];
                destination = data[2];
                break;
            case PC_BIG_DATA:
                dataLength = data[1];
                data += 2; 
                WPSENDLargeData(data, dataLength, totoalLength, destination, 0);
                break;
            case PC_BIG_DATA_END:
                WPSENDLargeData('1', 1, totoalLength, destination, 1);
                totoalLength = 0;
                destination = 0;
                break;
            case PC_BIG_DATA_READ:
            	WPSendData("a",1,NZP_DATA_READ,data[1],0);
            	break;
            default:
                break;
        }

    }
}

