//============================================================================
//文件名称：wp.c
//功能概要：无线通信及组网协议模块
//============================================================================
#include "wp.h"
#include "01_app_include.h"

#define ONE_SECONE_DELAY       200

char* encrypted(char* data){
	return data;
}


void WPSendData(char *data, char length, enum NZP_TYPE type, char destination, char needEncrypted) {
    if (needEncrypted)
        data = encrypted(data); //to do: encrypted
    char *p_data = NZP_v1_data(data, length, destination, type);

    rf_sentDataLength = length_of_NZP(p_data);

    RF_Init(255);           //RF模块初始化

    _lwmsgq_send((pointer)send_queue,p_data,LWMSGQ_SEND_BLOCK_ON_FULL);

}


void WPSENDLargeDataWithFrame(uint_8 *data, uint_8 length, char destination, uint_8 count) {

	uint_8 dataToSend[length+1];
	dataToSend[0] = count;
    memcpy(dataToSend+1, data, length);
    WPSendData(dataToSend, length+1, NZP_TS_DATA, destination, 0);
//    uart_send_string(UART_0,"After send TS_DATA");
	_time_delay_ticks(20);

}

// 返回 0 表示发送失败
// 返回 1 表示发送成功
uint_8 WPSENDLargeData(uint_8 *data, uint_8 length, uint_8 totalLength, char destination, uint_8 end) {
    static uint_8 canSendData = 0;
    static uint_8 frameCount = 0;  // 从 0 开始
    if (end) {
        canSendData = 0;
        frameCount = 0;
        // Lage_Data_Flag = CAN_NOT_SEND;
        WPSendData("a", 1, NZP_TS_END, destination, 0);
        uart_send_string(UART_0,"Frame end");
        return 1;
    }

    uint_8 i = 0;
    if (canSendData == 0) {
        WPSendData(&totalLength, 1, NZP_RTS, destination, 0);
//        uart_send_string(UART_0,"After send RTS");
        canSendData = 1;
        _time_delay_ticks(20);

    }

    uint_8 frameNums = length/MaxFrameLength;
    uint_8 lastFrameLength = length%MaxFrameLength;
    uint_8 *dataToSend;
    // if (lastFrameLength != 0) {
    //     frameNums += 1;
    // }
    for (i=0; i < frameNums; i+=1) {
        dataToSend = data + i*MaxFrameLength;
        WPSENDLargeDataWithFrame(dataToSend, MaxFrameLength, destination, frameCount);
        frameCount+=1;
    }
    if (lastFrameLength != 0) {
        dataToSend = data + i*MaxFrameLength;
        WPSENDLargeDataWithFrame(dataToSend, lastFrameLength, destination, frameCount);
        frameCount+=1;
    }
//    uart_send_string(UART_0,"Before return");
    return 1;
}


