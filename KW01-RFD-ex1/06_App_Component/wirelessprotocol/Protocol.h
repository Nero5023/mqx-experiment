//
//  Protocol.h
//  Protocol
//
//  Created by Nero Zuo on 17/3/29.
//  Copyright 婕 2017楠烇拷Nero. All rights reserved.
//


// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |     Source    |  Destination  |    Version    |     Type      |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           Length              |          Checksum             |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |     Data ...
// +-+-+-+-+-

#ifndef Protocol_h
#define Protocol_h

#include "common.h"    //閸栧懎鎯堥崗顒鍙＄憰浣虹婢跺瓨鏋冩禒锟
#include <stdio.h>
#define NZP_HEADER_LENGTH 8

typedef struct NZP_HEADER {
    char source;
    char destination;
    char version;
    char type;
    unsigned short length;
    unsigned short checksum;
} NZP_HEADER;

typedef enum NZP_TYPE {
    NZP_DATA  = 1,
    NZP_REGISTER = 2,
    NZP_REGISTER_Success = 3,
    NZP_TEMPERATURE = 4,
    NZP_TOUCH = 5,
    NZP_CONTIONUOUS_MONITOR = 6,
    NZP_HEARTBEAT = 7,
    NZP_RTS = 8,
    NZP_CTS = 9,
    NZP_TS_DATA = 10,
    NZP_ACK = 11
}NZP_TYPE;

//=====================================================================
//函数名称：NZP_v1_data
//函数返回：char* 生成的字符串地址
//参数说明：       data: 需要加入到 NZP 协议的数据的字符串首地址
//             length: data 字符串的长度
//        destination: 目标地址
//               type: 协议种类
//功能概要：将数据变成 NZP 协议的
//=====================================================================
char * NZP_v1_data(uint8_t *data, uint16_t length, uint8_t destination,  NZP_TYPE type);


//=====================================================================
//函数名称：data_length_of_NZP
//函数返回：int 返回数据长度
//参数说明：message: NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议下数据的长度
//=====================================================================
int data_length_of_NZP(char *message);


//=====================================================================
//函数名称：length_of_NZP
//函数返回：int 返回 NZP 协议报文长度
//参数说明：message: NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议报文的长度
//=====================================================================
int length_of_NZP(char *message);


//=====================================================================
//函数名称：parse_NZP
//函数返回：int 0：失败 1：成功
//参数说明：message: NZP 协议的字符串指针
//         length: 整个协议的长度
//           data: 将要返回的结果的字符串地址
//功能概要：将 NZP 协议的字符解析成数据，
//=====================================================================
int parse_NZP(char *message, int length, char* data);


//=====================================================================
//函数名称：type_of_NZP
//函数返回：NZP_TYPE 返回 NZP 协议报文类型
//参数说明：message: NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议报文的类型
//=====================================================================
NZP_TYPE type_of_NZP(char *message);

//=====================================================================
//函数名称：addr_of_NZP
//函数返回：uint_8 返回 NZP 协议的发送源节点地址
//参数说明：message: NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议发送源节点地址
//=====================================================================
uint_8 addr_of_NZP(char *message);

#endif /* Protocol_h */
