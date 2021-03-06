//===========================================================================
//文件名称：03_task_main.c（主任务函数）
//功能概要：放置MQXLite的自启动任务
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：根据需要更新
//===========================================================================
#include "01_app_include.h"
#define GLOBAL_VAR
//===========================================================================
//任务名称：task_main
//功能概要：MQX自启动任务，主要实现任务变量初始化、创建其他任务、安装用户ISR
//参数说明：未使用
//===========================================================================
void task_main(uint32_t initial_data)
{
	//1. 声明主任务使用的变量
	//1）创建轻量级事件组
	_lwevent_create(&lwevent_group,0);
	HD_adr=0xFF;

    SELF_ADDR = 0;
	//2. 关总中断
    DISABLE_INTERRUPTS;

    //3. 初始化外设模块
    uart_init(UART_0,MCGFLLCLK,9600);    //初始化UART_0模块
    light_init(LIGHT_RUN_1,LIGHT_OFF);   //初始化节点的运行指示灯1
    flash_init();                        //初始化Flash
    RF_Init(HD_adr);		         //RF模块初始化
    adc_init(SINGLE_END,8,SAMPLE16);

    //4. 给有关变量赋初值
    seconds=0;
    net_status = UNREGISTERED;
    is_continous_monitoring = 0;
    Lage_Data_Flag = CAN_NOT_SEND;
    light_control_value = 50;
    
	//初始化消息队列
	_lwmsgq_init((pointer)recv_queue,RECV_NUM_MESSAGES,RECV_MSG_SIZE);
	_lwmsgq_init((pointer)send_queue,SEND_NUM_MESSAGES,SEND_MSG_SIZE);
	_lwmsgq_init((pointer)flash_write_queue,FLASH_WRITE_NUM_MESSAGES,FLASH_WRITE_MSG_SIZE);
	_lwmsgq_init((pointer)flash_read_queue,FLASH_READ_NUM_MESSAGES,FLASH_READ_MSG_SIZE);
    //5. 创建其他任务
    _task_create_at(0, TASK_LIGHT,   0, task_light_stack,      TASK_LIGHT_STACK_SIZE);
    _task_create_at(0, TASK_RF_RECV, 0, task_rf_recv_stack,    TASK_RF_RECV_STACK_SIZE);
    _task_create_at(0, TASK_UART0_RE,0, task_uart0_Recv_stack, TASK_UART0_RECV_STACK_SIZE);
    _task_create_at(0, TASK_RF_SEND, 0, task_rf_send_stack,    TASK_RF_SEND_STACK_SIZE);
     _task_create_at(0, TASK_FLASH,   0, task_flash_stack,       TASK_FLASH_STACK_SIZE);
     _task_create_at(0, TASK_WP_REGISTER,0,task_wp_register_stack, TASK_WP_REGISTER_STACK_SIZE);
     _task_create_at(0, TASK_ADC,0,task_adc_stack, TASK_ADC_STACK_SIZE);

    //6. 安装用户ISR
    _int_install_isr(INT_UART0,isr_uart0_re,NULL);//串口0的ISR
    _int_install_isr(INT_PORTD,gpio_CD_ISR,NULL);     //PTC、PTD的ISR，端口C和D公用同一中断向量

    //7. 使能模块中断
	uart_enable_re_int(UART_0);   //使能串口0接收中断
	MKW01Drv_IRQ_DIO1_Enable();	  //开启DIO1中断

	//8. 开总中断
	ENABLE_INTERRUPTS;            //开总中断
    //------------执行完毕，本任务进入阻塞态----------------------------------------------

    _task_block();
}
