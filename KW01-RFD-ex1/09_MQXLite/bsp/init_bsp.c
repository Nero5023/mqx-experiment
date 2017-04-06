//============================================================================
// 文件名称: bsp_init.c
// 文件功能: 初始化BSP
// 更新日期: 2012-12-13
//============================================================================
#include "bsp.h"
#include "mqx_inc.h"

static void    _bsp_systick(pointer dummy);
static uint_32 _bsp_get_hwticks(pointer param);
static void    systick_init(void);

//============================================================================
// 函数名称：_bsp_enable_card
// 函数功能：初始化外设系统
// 输入参数：无
// 函数返回：函数执行状态
//============================================================================
uint_32 _bsp_enable_card ( void )
{
	register KERNEL_DATA_STRUCT_PTR kernel_data;
	uint_32    result;
	
	_GET_KERNEL_DATA(kernel_data); // 获得对kd的访问
	_mqx_set_cpu_type(MQX_CPU);    // 设定CPU类型
	
    // 初始化中断系统
	result = _psp_int_init(FIRST_INTERRUPT_VECTOR_USED, LAST_INTERRUPT_VECTOR_USED);
	if (result != MQX_OK) {
        return result;
    }
	
#if MQX_EXIT_ENABLED
    /* Set the bsp exit handler, called by _mqx_exit */
    _mqx_set_exit_handler(_bsp_exit_handler);
#endif
	
    // 向kd注册systick中断
    _time_set_timer_vector(BSP_TIMER_INTERRUPT_VECTOR);

// 暂时使用硬注册方式，在中断向量表里直接改systick中断
    
    // 注册systick中断服务函数到中断管理模块
    if (_int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, (void (_CODE_PTR_)(pointer))_bsp_systick, NULL) == NULL)
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    // 初始化系统定时器systick
    systick_init();
    
	// 登记系统时钟系统配置
	_GET_KERNEL_DATA(kernel_data);
	kernel_data->TIMER_HW_REFERENCE = (BSP_SYSTEM_CLOCK / BSP_ALARM_FREQUENCY);
	kernel_data->TICKS_PER_SECOND = BSP_ALARM_FREQUENCY;
	_time_set_hwticks_per_tick(kernel_data->TIMER_HW_REFERENCE);
	_time_set_hwtick_function(_bsp_get_hwticks, (pointer)NULL);

    return MQX_OK;
}

//============================================================================
// 函数名称：systick_init
// 函数功能：初始化系统定时器，用于产生tick
// 输入参数：无
// 函数返回：无
//============================================================================
static void systick_init(void)
{
	// 关闭SYSTICK
	SYST_CSR = 0;
	// 清除当前值计数器
	SYST_CVR = 0;
	// 设定倒计时计数值
	SYST_RVR = BSP_CORE_CLOCK / BSP_ALARM_FREQUENCY - 1;
	/* SVCall priority*/
	//SCB_SHPR2 |= 0x10000000; 
	// 设定 SysTick优先级
	SCB_SHPR3 |= SCB_SHPR3_PRI_15(CORTEX_PRIOR(BSP_TIMER_INTERRUPT_PRIORITY));
	// 使用内核时钟，倒计时到0时产生SYSTICK中断，使能SYSTICK
	SYST_CSR = 7;
}

//============================================================================
// 函数名称：_bsp_systick
// 函数功能：systick中断服务例程
// 输入参数：无意义
// 函数返回：无
//============================================================================
static void _bsp_systick(pointer dummy)
{
	_time_notify_kernel(); // MQX的滴答处理例程
}

//============================================================================
// 函数名称：_bsp_get_hwticks
// 函数功能：返回已经消逝的hw数量
// 输入参数：无意义
// 函数返回：已经消逝的hw数量
//============================================================================
static uint_32 _bsp_get_hwticks(pointer param) 
{
    uint_32 modulo;
    uint_32 count;
    uint_32 overflow;

    overflow = 0;
    modulo = (SYST_RVR & SysTick_RVR_RELOAD_MASK) + 1;
    count = SYST_CVR & SysTick_CVR_CURRENT_MASK;

    if (SCB_ICSR & SCB_ICSR_PENDSTSET_MASK) {
        /* Another full TICK period has expired... */
        count = SYST_CVR & SysTick_CVR_CURRENT_MASK;
        overflow = modulo;
    }

    /* interrupt flag is set upon 1->0 transition, not upon reload - wrap around */
    if (count == 0)
        count = modulo;

    return (modulo - count + overflow);
}
