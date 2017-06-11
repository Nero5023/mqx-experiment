#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�


typedef enum PC_COM_TYPE {
    PC_COM_NODES  = 'n',
    PC_COM_TEMP = 't',         // data[1] ���� ���͵�Ŀ�ĵ�
    PC_COM_CONTIONUOUS = 'c',  // data[1] ���� ���͵�Ŀ�ĵ�
    PC_BIG_DATA_START = 'l', // l | totoalLength | destination
    PC_BIG_DATA = 'b',       // d | dataLength | data
    PC_BIG_DATA_END = 'e',    // e |
    PC_BIG_DATA_READ = 'r',  // r | address
	PC_MISS_DATA     = 'M',  // M | addr |miss data length | data
	PC_LIGHT_CONTROL_MESSAGE = 'L' // L  | addr | value
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
        uint_8 frameOrder;
        switch (data[0]) {
            case PC_COM_NODES:        // ѯ�����еĽڵ�״̬
                aliveCount = alive_node_addrs(aliveAddrs); // ��ȡ���д��Ľڵ��ַ
                sendNodeStatus(aliveCount,aliveAddrs);     // ���� UART �� PC ����
                break;
            case PC_COM_TEMP:        // ѯ�ʽڵ��¶�
                des = data[1];       // ѯ�ʵ�ַΪ des �ڵ���¶���Ϣ
                WPSendData("a", 1, NZP_TEMPERATURE, des-'0', 0); // ����ѯ��Э��
                break;
            case PC_COM_CONTIONUOUS: // ��������¶���Ϣ
                des = data[1];
                WPSendData("a", 1, NZP_CONTIONUOUS_MONITOR, des-'0', 0);
                break;
            case PC_BIG_DATA_START:  // �����ݵ�ͷ֡
                totoalLength = data[1];  // �ܵ�֡��
                destination = data[2];   // Ŀ�ĵ�ַ
                WPSendData(&totoalLength, 1, NZP_RTS, destination, 0);
                break;
            case PC_BIG_DATA:       // �����ݵ�ÿһ֡
            	frameOrder = data[1];  // ֡��
                dataLength = data[2];  // ���ݳ���
                WPSENDLargeDataWithFrame(data+3, dataLength, destination, frameOrder);
                break;
            case PC_BIG_DATA_END:   // ����β
            	WPSendData("a", 1, NZP_TS_END, destination, 0);
                break;
            case PC_BIG_DATA_READ:  // ��ȡ�ڵ�� flash ������
            	WPSendData("a",1,NZP_DATA_READ,data[1],0);
            	break;
            case PC_MISS_DATA:  // ���Ͷ�ʧ����֡
            	// data[2]: the number of miss frames
            	destination = data[1]; // Ŀ��ڵ�
            	data[1] = 'M';
            	WPSendData(data+1, data[2]+2, NZP_ACK, destination, 0);
            	break;
            case PC_LIGHT_CONTROL_MESSAGE:  // ����С��Ƶ�ʿ���
            	destination = data[1];      // Ŀ��ڵ��ַ 
            	WPSendData(data+2, 1, NZP_LIGHT_CONTROL, destination, 0);
            	break;
            default:
                break;
        }

    }
}

