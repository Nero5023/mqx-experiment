/*
 * cp.h
 *
 *  Created on: 2017��4��13��
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


// ���ͽڵ�ע��ɹ���Ϣ
void sendNodeReisterSuccessInfo(char nodeAddr) ;

// �������߽ڵ���Ϣ
void sendNodeStatus(char numOfNodes, char* nodesAddrs) ;

// �����¶���Ϣ
void sendNodeTempInfo(char nodeAddr, char* tempFloat) ;

// ���ͽڵ㶪ʧ��Ϣ
void sendNodeDeathInfo(uint_8 nodeAddr);

// ���ʹ�����ͷ��Ϣ
void sendBigDataStart(uint_8 nodeAddr);

// ���ʹ����ݵ�һ֡����
void sendBigData(uint_8* data, uint_8 length);

// ���ʹ����ݵ�β��Ϣ
void sendBigDataEnd();

#endif
