//
//  Protocol.h
//  Protocol
//
//  Created by Nero Zuo on 17/3/29.
//  Copyright © 2017年 Nero. All rights reserved.
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

enum NZP_TYPE {
    NZP_DATA  = 1
};

//=====================================================================
//函数名称：NZP_v1_data
//函数返回：char* 生成的字符串地址，（需要 free!!!）
//参数说明：data：需要加入到 NZP 协议的数据的字符串首地址
//        length：data 字符串的长度
//        destination：目标地址
//功能概要：将数据变成 NZP 协议的
//=====================================================================
char * NZP_v1_data(char *data, unsigned short length, char destination, enum NZP_TYPE type);


//=====================================================================
//函数名称：data_length_of_NZP
//函数返回：int 返回数据长度
//参数说明：message：NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议下数据的长度
//=====================================================================
int data_length_of_NZP(char *message);


//=====================================================================
//函数名称：length_of_NZP
//函数返回：int 返回 NZP 协议报文长度
//参数说明：message：NZP 协议的字符串指针
//功能概要：通过传入的 message，返回 NZP 协议报文的长度
//=====================================================================
int length_of_NZP(char *message);

//=====================================================================
//函数名称：parse_NZP
//函数返回：int 0：失败 1：成功
//参数说明：message：NZP 协议的字符串指针
//         length: 整个协议的长度
//           data: 将要返回的结果的字符串地址
//功能概要：将 NZP 协议的字符解析成数据，
//=====================================================================
int parse_NZP(char *message, int length, char* data);

#endif /* Protocol_h */
