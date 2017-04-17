//
//  encrypt.h
//  Protocol
//
//  Created by Nero Zuo on 17/4/10.
//  Copyright © 2017年 Nero. All rights reserved.
//

#ifndef encrypt_h
#define encrypt_h

#include <stdio.h>

//=====================================================================
//函数名称：encode
//函数返回：无返回值
//参数说明： source: 需要加密的字符串指针
//           dest: 返回的加密数据
//         length: 字符串长度
//            key: 密钥
//功能概要：加密数据
//=====================================================================

void encode(char *source, char *dest, int length, char key);

//=====================================================================
//函数名称：decode
//函数返回：无返回值
//参数说明： cipher: 需要解密的字符串指针
//      clearText: 返回的解密数据
//         length: 字符串长度
//            key: 密钥
//功能概要：加密数据
//=====================================================================
void decode(char *cipher, char* clearText, int length, char key);

#endif /* encrypt_h */
