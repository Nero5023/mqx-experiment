//
//  address_manage.h
//  Register
//
//  Created by Nero Zuo on 17/4/26.
//  Copyright © 2017年 Nero. All rights reserved.
//

#ifndef address_manage_h
#define address_manage_h

#include "common.h"    //包含公共要素头文件

void init_reg_info();

uint_8 regisger();

void updateTTL();

void receive_heartbeat_from(uint_8 addr);

void receive_key_from(uint_8 addr, uint_8 key);

#endif /* address_manage_h */
