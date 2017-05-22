/*
 * cp.h
 *
 *  Created on: 2017年4月13日
 *      Author: 49738
 */

#ifndef CP_H
#define CP_H

#include "common.h"

typedef enum FFDDataType {
    RegisterSuccess = 's',  // s|node address
    NodeStatus    = 'n',       // n|num of nodes|node0|node1...
    TempInfo      = 't',        // t|node address|temp(float)
    NodeDeathInfo = 'd',  // d | node addr
    BigDataStart  = 'l',  // l | node addr      //the begin of bigdatafile
	BigDataEnd    = 'e',  // e | the end of bigdatafile
    BigData       = 'D'   // D | frame order | data
} FFDDataType;


// 发送节点注册成功消息
void sendNodeReisterSuccessInfo(char nodeAddr) ;

// 发送在线节点信息
void sendNodeStatus(char numOfNodes, char* nodesAddrs) ;

// 发送温度信息
void sendNodeTempInfo(char nodeAddr, char* tempFloat) ;

// 发送节点丢失信息
void sendNodeDeathInfo(uint_8 nodeAddr);

// 发送大数据头信息
void sendBigDataStart(uint_8 nodeAddr);

// 发送大数据的一帧数据
void sendBigData(uint_8* data, uint_8 length);

// 发送大数据的尾信息
void sendBigDataEnd();

#endif
