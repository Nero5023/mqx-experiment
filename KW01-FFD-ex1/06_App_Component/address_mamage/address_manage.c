#define REG_COUNT 10
#define DEFAULT_TTL 3

#include "address_manage.h"
#include "01_app_include.h"

void init_addr_info() {
    uint_8 i = 0;
    uint_8 j = 0;
    for (i = 0; i < REG_COUNT; i++) {
        for (j = 0; j < 3; j++) {
            ADDR_INFO[i][j] = 0;
        }
    }
}

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

void updateTTL() {
    uint_8 i = 0;
    for (i = 0; i < REG_COUNT; i++) {
        if (ADDR_INFO[i][0] != 0) {
            if (ADDR_INFO[i][2] == 0) { // need to delete this node
                // send uart to pc
                ADDR_INFO[i][0] = 0;
                ADDR_INFO[i][1] = 0;
            }else {
                ADDR_INFO[i][2] -= 1;
            }
        }
    }
}

void receive_heartbeat_from(uint_8 addr) {
    uint_8 index = addr - 1;
    ADDR_INFO[index][2] = DEFAULT_TTL;
}

void receive_key_from(uint_8 addr, uint_8 key) {
    uint_8 index = addr - 1;
    ADDR_INFO[index][1] = key;
    ADDR_INFO[index][2] = DEFAULT_TTL;
}