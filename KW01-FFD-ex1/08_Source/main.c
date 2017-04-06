//============================================================================
//文件名称：main.c      
//功能概要：主函数
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2014-4-22  V2.0
//============================================================================
#include "includes.h"

extern const MQXLITE_INITIALIZATION_STRUCT  MQX_init_struct;

int main(void)
{	
	// 初始化mqx
	_mqxlite_init((MQXLITE_INITIALIZATION_STRUCT_PTR) &MQX_init_struct);
    // 启动mqx
	_mqxlite();
	return 0;
}
