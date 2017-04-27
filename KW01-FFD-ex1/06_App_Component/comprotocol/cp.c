/*
 * cp.c
 *
 *  Created on: 2017年4月13日
 *      Author: 49738
 */

#include "cp.h"
#include "uart.h"

// 发送节点注册成功消息
void sendNodeReisterSuccessInfo(char nodeAddr) {
    char dataToSend[2] = {RegisterSuccess, nodeAddr};
    char dataLength = 2;
    uart_sendN(UART_0,dataLength,dataToSend);
}

// 发送在线节点信息
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

// 发送温度信息
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

// 发送节点丢失信息
void sendNodeDeathInfo(uint_8 nodeAddr) {
	char dataToSend[2];
	dataToSend[0] = NodeDeathInfo;
	dataToSend[1] = nodeAddr;
	uart_sendN(UART_0,2,dataToSend);
}
