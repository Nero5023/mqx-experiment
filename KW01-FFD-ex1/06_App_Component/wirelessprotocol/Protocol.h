//
//  Protocol.h
//  Protocol
//
//  Created by Nero Zuo on 17/3/29.
//  Copyright 漏 2017骞�Nero. All rights reserved.
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

#include "common.h"    //鍖呭惈鍏叡瑕佺礌澶存枃浠�
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
    NZP_REGISTER_Success = 3
}NZP_TYPE;

//=====================================================================
//鍑芥暟鍚嶇О锛歂ZP_v1_data
//鍑芥暟杩斿洖锛歝har* 鐢熸垚鐨勫瓧绗︿覆鍦板潃锛岋紙闇� free!!!锛�
//鍙傛暟璇存槑锛歞ata锛氶渶瑕佸姞鍏ュ埌 NZP 鍗忚鐨勬暟鎹殑瀛楃涓查鍦板潃
//        length锛歞ata 瀛楃涓茬殑闀垮害
//        destination锛氱洰鏍囧湴鍧�
//鍔熻兘姒傝锛氬皢鏁版嵁鍙樻垚 NZP 鍗忚鐨�
//=====================================================================
char * NZP_v1_data(uint8_t *data, uint16_t length, uint8_t destination,  NZP_TYPE type);


//=====================================================================
//鍑芥暟鍚嶇О锛歞ata_length_of_NZP
//鍑芥暟杩斿洖锛歩nt 杩斿洖鏁版嵁闀垮害
//鍙傛暟璇存槑锛歮essage锛歂ZP 鍗忚鐨勫瓧绗︿覆鎸囬拡
//鍔熻兘姒傝锛氶�杩囦紶鍏ョ殑 message锛岃繑鍥�NZP 鍗忚涓嬫暟鎹殑闀垮害
//=====================================================================
int data_length_of_NZP(char *message);


//=====================================================================
//鍑芥暟鍚嶇О锛歭ength_of_NZP
//鍑芥暟杩斿洖锛歩nt 杩斿洖 NZP 鍗忚鎶ユ枃闀垮害
//鍙傛暟璇存槑锛歮essage锛歂ZP 鍗忚鐨勫瓧绗︿覆鎸囬拡
//鍔熻兘姒傝锛氶�杩囦紶鍏ョ殑 message锛岃繑鍥�NZP 鍗忚鎶ユ枃鐨勯暱搴�
//=====================================================================
int length_of_NZP(char *message);

//=====================================================================
//鍑芥暟鍚嶇О锛歱arse_NZP
//鍑芥暟杩斿洖锛歩nt 0锛氬け璐�1锛氭垚鍔�
//鍙傛暟璇存槑锛歮essage锛歂ZP 鍗忚鐨勫瓧绗︿覆鎸囬拡
//         length: 鏁翠釜鍗忚鐨勯暱搴�
//           data: 灏嗚杩斿洖鐨勭粨鏋滅殑瀛楃涓插湴鍧�
//鍔熻兘姒傝锛氬皢 NZP 鍗忚鐨勫瓧绗﹁В鏋愭垚鏁版嵁锛�
//=====================================================================
int parse_NZP(char *message, int length, char* data);


//=====================================================================
//鍑芥暟鍚嶇О锛歵ype_of_NZP
//鍑芥暟杩斿洖锛歂ZP_TYPE 杩斿洖鏁版嵁鐨勭被鍨�
//鍙傛暟璇存槑锛歮essage锛歂ZP 鍗忚鐨勫瓧绗︿覆鎸囬拡
//鍔熻兘姒傝锛氶�杩囦紶鍏ョ殑 message锛岃繑鍥�NZP 鍗忚鐨勭被鍨�
//=====================================================================
NZP_TYPE type_of_NZP(char *message);



#endif /* Protocol_h */
