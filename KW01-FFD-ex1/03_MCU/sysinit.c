#include "sysinit.h"



//===========================================================================
//函数名称：sys_init
//函数返回：无
//参数说明：无 
//功能概要：该函数禁止看门狗模块，在实际项目中，需打开看门狗。采用了外部晶振为8Mhz的输
//          入，输出内核时钟48MHz，总线时钟24MHz。 
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
	  /* SIM_SOPT2: PLLFLLSEL=0 ，初步选择FLL作为外设时钟源*/
	  SIM_SOPT2 &= (uint32_t)~(uint32_t)(SIM_SOPT2_PLLFLLSEL_MASK); /* Select FLL as a clock source for various peripherals */
	  /* SIM_SOPT1: OSC32KSEL=0 ，选择32kHZ作为外设频率*/
	  SIM_SOPT1 &= (uint32_t)~(uint32_t)(SIM_SOPT1_OSC32KSEL(0x03)); /* System oscillator drives 32 kHz clock for various peripherals */
	  /* SIM_SOPT2: TPMSRC=1 ，选择TPM模块的时钟源*/
	  SIM_SOPT2 = (uint32_t)
				  (
						  (  SIM_SOPT2 & (uint32_t)~(uint32_t)  (  SIM_SOPT2_TPMSRC(0x02)  )  )
						  | (uint32_t)(  SIM_SOPT2_TPMSRC(0x01)  )
	              );                      /* Set the TPM clock */
	  /* Switch to FEI Mode ,FEI频率误差指标*/
	  /* MCG_C1: CLKS=0：输出FLL或PLL（具体取决于PLLS控制位）,FRDIV=0：,IREFS=1：选择内部低参考时钟,
	   * IRCLKEN=1：MCGIR_CLK激活,IREFSTEN=0停止（低功耗）模式下关内部参考时钟 */
	  MCG_C1 = (MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK);     
//		  MCG_C1 = (MCG_C1_IREFS_MASK | MCG_C1_IRCLKEN_MASK | MCG_C1_CLKS(0x01)); 
	  /* MCG_C2: LOCRE0=0,??=0,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 */
//		  MCG_C2 = 0x00U;   
	  //选择4MHz内部时钟
	  MCG_C2 = (0x00U | MCG_C2_IRCS_MASK);
	  /* MCG_C4: DMX32=0,DRST_DRS=0 */
	  MCG_C4 &= (uint8_t)~(uint8_t)((MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x03)));                                                   
	  /* OSC0_CR: ERCLKEN=1:使能外部时钟,??=0,EREFSTEN=0,??=0,SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
	  OSC0_CR = OSC_CR_ERCLKEN_MASK;                                                   
	  /* MCG_C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 1分频*/
	  MCG_C5 = 0x00U;                                                   
	  /* MCG_C6: LOLIE0=0,PLLS=0：选择FLL,CME0=0,VDIV0=0 24倍频*/
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
		SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; //设置PLL/2=48M/2作为外设时钟源
		/* Enable the CLKOUT function on PTC3 (alt5 function) */
		PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
		/* Select the CLKOUT in the SMI_SOPT2 mux to be MCGIR_CLK ：010 -- Bus clock   100 MCGIR_CLK*/
		SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(0x02);	
}





