//
//  address_manage.h
//  Register
//
//  Created by Nero Zuo on 17/4/26.
//  Copyright 漏 2017骞�Nero. All rights reserved.
//

#ifndef address_manage_h
#define address_manage_h


#include "common.h"    //鍖呭惈鍏叡瑕佺礌澶存枃浠�


#define REG_COUNT 10

void init_addr_info();

uint_8 regisger();

void updateTTL();

void receive_heartbeat_from(uint_8 addr);

void receive_key_from(uint_8 addr, uint_8 key);

uint_8 alive_node_addrs(uint_8* aliveAddrs);

#endif /* address_manage_h */
