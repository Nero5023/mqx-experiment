//
//  encrypt.c
//  Protocol
//
//  Created by Nero Zuo on 17/4/10.
//  Copyright © 2017年 Nero. All rights reserved.
//

#include "encrypt.h"

void encode(char *source, char *dest, int length, char key) {
    int i = 0;
    for (i = 0; i < length; i++) {
        dest[i] = (source[i] + key) % 256;
    }
}

void decode(char *cipher, char* clearText, int length, char key) {
    int i = 0;
    for (i = 0; i < length; i++) {
        clearText[i] = (cipher[i] - key) % 256;
    }
}
