#define DEFAULT_TTL 3

#include "address_manage.h"
#include "01_app_include.h"

//=====================================================================
//函数名称：init_addr_info
//函数返回：无
//参数说明：无
//功能概要：初始化地址
//=====================================================================
void init_addr_info() {
    uint_8 i = 0;
    uint_8 j = 0;
    for (i = 0; i < REG_COUNT; i++) {
        for (j = 0; j < 3; j++) {
            ADDR_INFO[i][j] = 0;
        }
    }
}

//=====================================================================
//函数名称：regisger
//函数返回：注册得到的地址
//参数说明：无
//功能概要：注册一个地址，返回注册的地址
//=====================================================================
uint_8 regisger() {
    uint_8 i = 0;
    for (i = 0; i < REG_COUNT; i++) {
        if (ADDR_INFO[i][0] == 0) {
            ADDR_INFO[i][0] = i+1;
            ADDR_INFO[i][2] = 3;
            return i+1;
        }
    }
    return 0;
}

//=====================================================================
//函数名称：updateTTL
//函数返回：无
//参数说明：无
//功能概要：更新各个地址的 Time To Live 值
//=====================================================================
void updateTTL() {
    uint_8 i = 0;
    for (i = 0; i < REG_COUNT; i++) {
        if (ADDR_INFO[i][0] != 0) {
            if (ADDR_INFO[i][2] == 0) { // need to delete this node
                // send uart to pc
            	sendNodeDeathInfo(ADDR_INFO[i][0]);
                ADDR_INFO[i][0] = 0;
                ADDR_INFO[i][1] = 0;

            }else {
                ADDR_INFO[i][2] -= 1;
            }
        }
    }
}

//=====================================================================
//函数名称：receive_heartbeat_from
//函数返回：无
//参数说明：addr: 获得来自 addr 的心跳包
//功能概要：获取来自 addr 的心跳包然后更新 TTL
//=====================================================================
void receive_heartbeat_from(uint_8 addr) {
	if (addr <=0 || addr>= 11) {
		return;
	}
    uint_8 index = addr - 1;
    if (ADDR_INFO[index][0] != 0) {
    	ADDR_INFO[index][2] = DEFAULT_TTL;
    }
}

//=====================================================================
//函数名称：receive_key_from
//函数返回：无
//参数说明：addr: 源节点的地址
//         key: addr 的密钥
//功能概要：获取来自 addr 的密钥
//=====================================================================
void receive_key_from(uint_8 addr, uint_8 key) {
    uint_8 index = addr - 1;
    ADDR_INFO[index][1] = key;
    ADDR_INFO[index][2] = DEFAULT_TTL;
}

//=====================================================================
//函数名称：alive_node_addrs
//函数返回：存活的节点个数
//参数说明：aliveAddrs: 传入一个 array，函数执行后，会将活着的地址放到改数组中
//功能概要：获得所有活着的节点
//=====================================================================
uint_8 alive_node_addrs(uint_8* aliveAddrs) {
	uint_8 count = 0;
	uint_8 i;
	for (i = 0; i < REG_COUNT; i++) {
		if (ADDR_INFO[i][0] != 0) {
			aliveAddrs[count] = ADDR_INFO[i][0];
			count+=1;
		}
	}
	return count;
}
