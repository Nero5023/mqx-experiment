//============================================================================
// �ļ�����: bsp_init.c
// �ļ�����: ��ʼ��BSP
// ��������: 2012-12-13
//============================================================================
#include "bsp.h"
#include "mqx_inc.h"

static void    _bsp_systick(pointer dummy);
static uint_32 _bsp_get_hwticks(pointer param);
static void    systick_init(void);

//============================================================================
// �������ƣ�_bsp_enable_card
// �������ܣ���ʼ������ϵͳ
// �����������
// �������أ�����ִ��״̬
//============================================================================
uint_32 _bsp_enable_card ( void )
{
	register KERNEL_DATA_STRUCT_PTR kernel_data;
	uint_32    result;
	
	_GET_KERNEL_DATA(kernel_data); // ��ö�kd�ķ���
	_mqx_set_cpu_type(MQX_CPU);    // �趨CPU����
	
    // ��ʼ���ж�ϵͳ
	result = _psp_int_init(FIRST_INTERRUPT_VECTOR_USED, LAST_INTERRUPT_VECTOR_USED);
	if (result != MQX_OK) {
        return result;
    }
	
#if MQX_EXIT_ENABLED
    /* Set the bsp exit handler, called by _mqx_exit */
    _mqx_set_exit_handler(_bsp_exit_handler);
#endif
	
    // ��kdע��systick�ж�
    _time_set_timer_vector(BSP_TIMER_INTERRUPT_VECTOR);

// ��ʱʹ��Ӳע�᷽ʽ�����ж���������ֱ�Ӹ�systick�ж�
    
    // ע��systick�жϷ��������жϹ���ģ��
    if (_int_install_isr(BSP_TIMER_INTERRUPT_VECTOR, (void (_CODE_PTR_)(pointer))_bsp_systick, NULL) == NULL)
    {
        return MQX_TIMER_ISR_INSTALL_FAIL;
    }

    // ��ʼ��ϵͳ��ʱ��systick
    systick_init();
    
	// �Ǽ�ϵͳʱ��ϵͳ����
	_GET_KERNEL_DATA(kernel_data);
	kernel_data->TIMER_HW_REFERENCE = (BSP_SYSTEM_CLOCK / BSP_ALARM_FREQUENCY);
	kernel_data->TICKS_PER_SECOND = BSP_ALARM_FREQUENCY;
	_time_set_hwticks_per_tick(kernel_data->TIMER_HW_REFERENCE);
	_time_set_hwtick_function(_bsp_get_hwticks, (pointer)NULL);

    return MQX_OK;
}

//============================================================================
// �������ƣ�systick_init
// �������ܣ���ʼ��ϵͳ��ʱ�������ڲ���tick
// �����������
// �������أ���
//============================================================================
static void systick_init(void)
{
	// �ر�SYSTICK
	SYST_CSR = 0;
	// �����ǰֵ������
	SYST_CVR = 0;
	// �趨����ʱ����ֵ
	SYST_RVR = BSP_CORE_CLOCK / BSP_ALARM_FREQUENCY - 1;
	/* SVCall priority*/
	//SCB_SHPR2 |= 0x10000000; 
	// �趨 SysTick���ȼ�
	SCB_SHPR3 |= SCB_SHPR3_PRI_15(CORTEX_PRIOR(BSP_TIMER_INTERRUPT_PRIORITY));
	// ʹ���ں�ʱ�ӣ�����ʱ��0ʱ����SYSTICK�жϣ�ʹ��SYSTICK
	SYST_CSR = 7;
}

//============================================================================
// �������ƣ�_bsp_systick
// �������ܣ�systick�жϷ�������
// ���������������
// �������أ���
//============================================================================
static void _bsp_systick(pointer dummy)
{
	_time_notify_kernel(); // MQX�ĵδ�������
}

//============================================================================
// �������ƣ�_bsp_get_hwticks
// �������ܣ������Ѿ����ŵ�hw����
// ���������������
// �������أ��Ѿ����ŵ�hw����
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
