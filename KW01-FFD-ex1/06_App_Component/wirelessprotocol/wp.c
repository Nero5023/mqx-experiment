//============================================================================
//�ļ����ƣ�wp.c
//���ܸ�Ҫ������ͨ�ż�����Э��ģ��
//============================================================================
#include "wp.h"
#include "01_app_include.h"
char* encrypted(char* data){
	return data;
}


void WPSendData(char *data, char length, enum NZP_TYPE type, char destination, char needEncrypted) {
    if (needEncrypted)
        data = encrypted(data); //to do: encrypted
    char *p_data = NZP_v1_data(data, length, destination, type);

    rf_sentDataLength = length_of_NZP(p_data);

    RF_Init(255);           //RFģ���ʼ��

    _lwmsgq_send((pointer)send_queue,p_data,0);

}

