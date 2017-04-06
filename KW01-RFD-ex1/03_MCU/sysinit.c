#include "sysinit.h"



//===========================================================================
//�������ƣ�sys_init
//�������أ���
//����˵������ 
//���ܸ�Ҫ���ú�����ֹ���Ź�ģ�飬��ʵ����Ŀ�У���򿪿��Ź����������ⲿ����Ϊ8Mhz����
//          �룬����ں�ʱ��48MHz������ʱ��24MHz�� 
//===========================================================================
void sys_init(void)  
{

	  /*** !!! Here you can place your own code before PE initialization using property "User code before PE initialization" on the build options tab. !!! ***/

	  /*** ### MKL25Z128VLK4 "Cpu" init code ... ***/
	  /*** PE initialization code after reset ***/
//		  SCB_VTOR = (uint32_t)(&__vect_table); /* Set the interrupt vector table position */
	  /* Disable the WDOG module */
	  /* SIM_COPC: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,COPT=0,COPCLKS=0,COPW=0 */
	  SIM_COPC = 0x00U;                                                   
	  /* System clock initialization */
	  /* SIM_SCGC5: PORTE=1,PORTA=1 */
	  SIM_SCGC5 |= (SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTA_MASK); /* Enable clock gate for ports to enable pin routing */
	  /* SIM_CLKDIV1: OUTDIV1=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	  SIM_CLKDIV1 = 0x00U;                 /* Update system prescalers */
	  /* SIM_SOPT2: PLLFLLSEL=0 ������ѡ��FLL��Ϊ����ʱ��Դ*/
	  SIM_SOPT2 &= (uint32_t)~(uint32_t)(SIM_SOPT2_PLLFLLSEL_MASK); /* Select FLL as a clock source for various peripherals */
	  /* SIM_SOPT1: OSC32KSEL=0 ��ѡ��32kHZ��Ϊ����Ƶ��*/
	  SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
	  /* SIM_SOPT2: TPMSRC=1 ��ѡ��TPMģ���ʱ��Դ*/
	  SIM_SOPT2 = (uint32_t)
				  (
						  (  SIM_SOPT2 & (uint32_t)~(uint32_t)  (  SIM_SOPT2_TPMSRC(0x02)  )  )
						  | (uint32_t)(  SIM_SOPT2_TPMSRC(0x01)  )
	              );                      /* Set the TPM clock */
	  /* Switch to FEI Mode ,FEIƵ�����ָ��*/
	  /* MCG_C1: CLKS=0�����FLL��PLL������ȡ����PLLS����λ��,FRDIV=0��,IREFS=1��ѡ���ڲ��Ͳο�ʱ��,
	   * IRCLKEN=1��MCGIR_CLK����,IREFSTEN=0ֹͣ���͹��ģ�ģʽ�¹��ڲ��ο�ʱ�� */
	  MCG_C1 = (MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK);     
//		  MCG_C1 = (MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK | MCG_C1_CLKS(0x01)); 
	  /* MCG_C2: LOCRE0=0,??=0,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 */
//		  MCG_C2 = 0x00U;   
	  //ѡ��4MHz�ڲ�ʱ��
	  MCG_C2 = (0x00U | MCG_C2_IRCS_MASK);
	  /* MCG_C4: DMX32=0,DRST_DRS=0 */
	  MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));                                                   
	  /* OSC0_CR: ERCLKEN=1:ʹ���ⲿʱ��,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
	  OSC0_CR = OSC_CR_ERCLKEN_MASK;                                                   
	  /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 1��Ƶ*/
	  MCG_C5 = 0x00U;                                                   
	  /* MCG_C6: LOLIE0=0,PLLS=0��ѡ��FLL,CME0=0,VDIV0=0 24��Ƶ*/
	  MCG_C6 = 0x00U;                                                   
	  while((MCG_S & MCG_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
	  }
	  while((MCG_S & 0x0CU) != 0x00U) {    /* Wait until output of the FLL is selected */
	  }
	  /*** End of PE initialization code after reset ***/
	
	  /*** !!! Here you can place your own code after PE initialization using property "User code after PE initialization" on the build options tab. !!! ***/
  
		SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
				  | SIM_SCGC5_PORTB_MASK
				  | SIM_SCGC5_PORTC_MASK
				  | SIM_SCGC5_PORTD_MASK
				  | SIM_SCGC5_PORTE_MASK );
		SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; //����PLL/2=48M/2��Ϊ����ʱ��Դ
		/* Enable the CLKOUT function on PTC3 (alt5 function) */
		PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
		/* Select the CLKOUT in the SMI_SOPT2 mux to be MCGIR_CLK ��010 -- Bus clock   100 MCGIR_CLK*/
		SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(0x02);	
}





