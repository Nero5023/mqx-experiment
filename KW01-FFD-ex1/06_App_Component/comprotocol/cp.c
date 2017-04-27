/*
 * cp.c
 *
 *  Created on: 2017��4��13��
 *      Author: 49738
 */

#include "cp.h"
#include "uart.h"

// ���ͽڵ�ע��ɹ���Ϣ
void sendNodeReisterSuccessInfo(char nodeAddr) {
    char dataToSend[2] = {RegisterSuccess, nodeAddr};
    char dataLength = 2;
    uart_sendN(UART_0,dataLength,dataToSend);
}

// �������߽ڵ���Ϣ
void sendNodeStatus(char numOfNodes, char* nodesAddrs) {
    char dataToSend[2+numOfNodes];
    int i = 0;
    dataToSend[0] = NodeStatus;
    dataToSend[1] = numOfNodes;
    for (i = 0; i < numOfNodes; i++) {
        dataToSend[i+2] = nodesAddrs[i];
    }
    char dataLength = 2 + numOfNodes;
   	uart_sendN(UART_0,dataLength,dataToSend);
}

// �����¶���Ϣ
void sendNodeTempInfo(char nodeAddr, char* tempFloat) {
    char dataLength = 2+4;
    int i = 0;
    char dataToSend[dataLength];
    dataToSend[0] = TempInfo;
    dataToSend[1] = nodeAddr;
    for (i = 0; i < 4; i++) {
        dataToSend[i+2] = tempFloat[i];
    }
    uart_sendN(UART_0,dataLength,dataToSend);
}

// ���ͽڵ㶪ʧ��Ϣ
void sendNodeDeathInfo(uint_8 nodeAddr) {
	char dataToSend[2];
	dataToSend[0] = NodeDeathInfo;
	dataToSend[1] = nodeAddr;
	uart_sendN(UART_0,2,dataToSend);
}
