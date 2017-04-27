/*
 * task_adc.c
 *
 *  Created on: 2016��3��24��
 *      Author: asus
 */
#include "01_app_include.h"    //Ӧ�����񹫹�ͷ�ļ�

//===========================================================================
//�������ƣ�task_adc
//���ܸ�Ҫ����ȡ26ͨ�����¶���������������¶�ֵ��
//                ���������ȼ�Ϊ14
//����˵����δʹ��
//===========================================================================
void task_adc(uint32_t initial_data)
{
	//1. ��������ʹ�õı���
	uint_16 temp;        //���ڴ洢�¶��������Ķ�ȡ
	static float temp_adc=0;
	//2. ���йر�������ֵ

	//3. ��������ѭ����
	while(TRUE)
	{
		//��ȡ9ͨ����������
		g_temperature=adc_read(9);
		if(is_continous_monitoring==1 && abs(g_temperature-temp_adc)>20){
			WPSendData(&g_temperature,4,NZP_TEMPERATURE,PC_NODE_ADDR,0);
			temp_adc = g_temperature;
		}
		//2��������ʱ
		_time_delay_ticks(100); //��ʱ200*5ms=1s
	}
	//����ѭ����end_while
}




