/*
 * cp.c
 *
 *  Created on: 2017年4月13日
 *      Author: 49738
 */

#include "cp.h"
#include "uart.h"

#define UART_FRAME_HEAD 0x05
#define UART_FRAME_TAIL 0x03



void MyUartSendN(uint_8 datalength,uint_8* data){
	uart_send1(UART_0,UART_FRAME_HEAD);
	uart_send1(UART_0,datalength);

	uart_sendN(UART_0,datalength,data);

	uart_send1(UART_0,UART_FRAME_TAIL);
}


// 发送节点注册成功消息
void sendNodeReisterSuccessInfo(char nodeAddr) {
    char dataToSend[2] = {RegisterSuccess, nodeAddr};
    char dataLength = 2;
    MyUartSendN(dataLength,dataToSend);
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
    MyUartSendN(dataLength,dataToSend);
//   	uart_sendN(UART_0,dataLength,dataToSend);
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
    MyUartSendN(dataLength,dataToSend);
//    uart_sendN(UART_0,dataLength,dataToSend);
}

// 发送节点丢失信息
void sendNodeDeathInfo(uint_8 nodeAddr) {
	char dataToSend[2];
	dataToSend[0] = NodeDeathInfo;
	dataToSend[1] = nodeAddr;
	MyUartSendN(2,dataToSend);
//	uart_sendN(UART_0,2,dataToSend);
}

// 发送大数据头信息
void sendBigDataStart(uint_8 nodeAddr, uint_8 totalFrameCount) {
    uint_8 dataToSend[3];
    dataToSend[0] = BigDataStart;
    dataToSend[1] = nodeAddr;
    dataToSend[2] = totalFrameCount;
    MyUartSendN(3,dataToSend);
//    uart_sendN(UART_0,2,dataToSend);
}

// 发送大数据的一帧数据
void sendBigData(uint_8* data, uint_8 length) {
    uint_8 dataToSend[1+length];
    dataToSend[0] = BigData;
    memcpy((dataToSend+1), data, length);
    MyUartSendN(length+1,dataToSend);
//    uart_sendN(UART_0, length+1, dataToSend);
}

// 发送大数据的尾信息
void sendBigDataEnd() {
	uint_8 dataToSend[1];
	dataToSend[0] = BigDataEnd;
	MyUartSendN(1,dataToSend);
//	 uart_sendN(UART_0, 1, dataToSend);
}


void sendBigDataMiss(uint_8* data){

	//  BigDataMiss | miss number | miss frame order
	uint_8 dataToSend[data[0]+2];
	dataToSend[0] = BigDataMiss;
	memcpy(dataToSend+1,data,data[0]+1);
	MyUartSendN(data[0]+2,dataToSend);

}
