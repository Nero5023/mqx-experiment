//
//  Protocol.c
//  Protocol
//
//  Created by Nero Zuo on 17/3/29.
//  Copyright 漏 2017骞�Nero. All rights reserved.
//

#include "Protocol.h"
#include <stdlib.h>
#include "01_app_include.h"

#define SELF_ADDR 0b00001111

#define NZP_DATA_TYPE 1



#define NZP_V1 1

char const BROADCAST_ADDR = 0b11111111;

unsigned short checksum(void *b, int len);
void strcat_header_data(char *res, char const *header, char const *data, int datalength);
void header_struct_to_str(NZP_HEADER* header, char *headStr);

// make the NZP protocal data
// length is the length of the data not include the header
char * NZP_v1_data(uint8_t *data, uint16_t length, uint8_t destination, NZP_TYPE type) {
    NZP_HEADER header;
    header.source = SELF_ADDR;
    header.destination = destination;
    header.version = 1;
    header.type = type;
    header.length = length;
    header.checksum = 0;
    
    char headerStr[NZP_HEADER_LENGTH];
    header_struct_to_str(&header, headerStr);
    // char *res = malloc(NZP_HEADER_LENGTH+length);
    strcat_header_data(rf_sentBuf, headerStr, data, length);
    header.checksum = checksum(rf_sentBuf, NZP_HEADER_LENGTH + length);
    
    char newHeaderStr[NZP_HEADER_LENGTH];
    header_struct_to_str(&header, newHeaderStr);
    strcat_header_data(rf_sentBuf, newHeaderStr, data, length);
    return rf_sentBuf;
}



// make the header struct to string
void header_struct_to_str(NZP_HEADER* header, char *headStr) {
    memcpy(headStr, header, NZP_HEADER_LENGTH);
}


// assemble the struct header and data
// copy header and data to res
void strcat_header_data(char *res, char const *header, char const *data, int datalength) {
//    char res[NZP_HEADER_LENGTH + datalength];
    int i = 0;
    for(i = 0; i < NZP_HEADER_LENGTH; i++) {
        res[i] = header[i];
    }
    for (i = NZP_HEADER_LENGTH; i < datalength + NZP_HEADER_LENGTH; i++) {
        res[i] = data[i-NZP_HEADER_LENGTH];
    }
}

// compute the checksum
unsigned short checksum(void *b, int len){
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;
    
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


// parse

int version_of_NZP(char *message) {
    NZP_HEADER *h = (NZP_HEADER *)message;
    return h->version;
}

// the data length of NZP
int data_length_of_NZP(char *message) {
    NZP_HEADER *h = (NZP_HEADER *)message;
    return h->length;
}

int length_of_NZP(char *message) {
    int data_length = data_length_of_NZP(message);
    return data_length + NZP_HEADER_LENGTH;
}


// check if send to self
int check_if_send_to_self(char *message) {
    NZP_HEADER *h = (NZP_HEADER *)message;
    if (h->destination == BROADCAST_ADDR) {
        return 1;
    }
    if (h->destination == SELF_ADDR) {
        return 1;
    }
    return 0;
}

// total length
int check_data_is_correct(char *message, int length) {
    NZP_HEADER *h = (NZP_HEADER *)message;
    if (h->length != length - NZP_HEADER_LENGTH)
        return 0;
    if (checksum(message, length) == 0) {
        // success
        return 1;
    }
    return 0;
}

// parse nzp protocol 1
// length not use
void parse_NZP_v1(char *message, int length, char *data) {
    // data = message + NZP_HEADER_LENGTH;
    memcpy(data, message + NZP_HEADER_LENGTH, length-NZP_HEADER_LENGTH);
}


// 1 : success
// 0 : failed
// length: 鏁翠釜鍗忚鐨勯暱搴�
int parse_NZP(char *message, int length, char *data) {
    if (check_if_send_to_self(message) == 0)
        return 0;
    
    if (check_data_is_correct(message, length) != 1)
        return 0;
    
    if (version_of_NZP(message) == NZP_V1) {
        parse_NZP_v1(message, length, data);
        return 1;
    }
    return 0;
}

int data_length_in_NZP_v1(int message_length) {
    return message_length - NZP_HEADER_LENGTH;
}


// return the type of message
NZP_TYPE type_of_NZP(char *message) {
    NZP_HEADER *h = (NZP_HEADER *)message;
    return h->type;
}

