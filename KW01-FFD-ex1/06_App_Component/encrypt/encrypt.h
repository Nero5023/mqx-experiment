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

void encode(char *source, char *dest, int length, char key);

void decode(char *cipher, char* clearText, int length, char key);


#endif /* encrypt_h */
