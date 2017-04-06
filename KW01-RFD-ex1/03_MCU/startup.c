
#include "common.h"
#include "sysinit.h"
#include "rf.h"
#include "spi.h"

#define	MKW01_Reg_DioMapping2	  	0x26//Mapping	of	pins	DIO4	and	DIO5,	ClkOut	frequency		0x05	0x07
#define	MKW01_Reg_PaRamp	        0x12//	Control	of	the	PA	ramp	time	in	FSK	mode	0x09		

extern int main(void);
//�����ڲ�����
static void vector_init(void);
static void m_zero_fill_bss(void);
static void m_data_seg_init(void);
void sys_pin_enable_port(void);



void Platform_Init(void);
void MCG_Setup(void);
int pll_init(int crystal_val, unsigned char hgo_val,\
			unsigned char erefs_val, signed char prdiv_val,\
			signed char vdiv_val, unsigned char mcgout_select);

//���Ź�WDOG(COP)�궨��
#define WDOG_DISABLE() {SIM_COPC = 0x00u;}  //���ÿ��Ź�
//COPʹ��LPOʱ�ӣ�COP��2^10��LPOʱ�����ں���ʱ
#define WDOG_ENABLE() {SIM_COPC |= SIM_COPC_COPT_MASK;} //ʹ�ܿ��Ź�

void startup(void)
{

	WDOG_DISABLE(); //�ؿ��Ź�
	
//	vector_init();          //�����ж���������RAM��GZF 2014-4-23��
	m_zero_fill_bss();      //��bss��
	m_data_seg_init();      //��ROM�еĳ�ʼ�����ݿ�����RAM��
//	sys_init();
	sys_pin_enable_port();  //ʹ�����ж˿�ʱ�ӣ��򿪶�Ӧ�˿ڵ�ʱ���ţ��Ա����������Ÿ��ù���
	
	DISABLE_INTERRUPTS;     //���жϣ������ٽ���
	
	Platform_Init();        //��ʼ��MCU��RF
	
	ENABLE_INTERRUPTS;      //���жϣ��˳��ٽ���

	main();
}

//�����ж�������
void vector_init(void)
{
	extern char __VECTOR_RAM[];
	extern char __vector_table[];
	uint_32 n;
	
	if (__VECTOR_RAM != __vector_table)
	{
	    for (n = 0; n < 0x410; n++)
	        __VECTOR_RAM[n] = __vector_table[n];
	}
	//Point the VTOR to the new copy of the vector table 
      SCB_VTOR=(uint_32)__VECTOR_RAM;
}

// ��ʼ��BSS��
void m_zero_fill_bss(void)
{
	extern char __START_BSS[];
    extern char __END_BSS[];

    uint_8 *bss_start = (uint_8 *)__START_BSS;
    uint_8 *bss_end   = (uint_8 *)__END_BSS;
    uint_32 n = bss_end - bss_start;
    
    while (n--)
    	*bss_start++ = 0;
}

void m_data_seg_init(void)
{
	extern char _sdata[];
	extern char _edata[];
	extern char _etext[];
	
	uint_8 *data_start = (uint_8 *)_sdata;
	uint_8 *data_end   = (uint_8 *)_edata;
	uint_8 *data_rom_start = (uint_8 *)_etext;
	uint_32 n = data_end - data_start;
	
	while (n--)
	    *data_start++ = *data_rom_start++;
}

void sys_pin_enable_port(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK
			  | SIM_SCGC5_PORTB_MASK
			  | SIM_SCGC5_PORTC_MASK
			  | SIM_SCGC5_PORTD_MASK
			  | SIM_SCGC5_PORTE_MASK;
}
 

void Platform_Init(void)
{
	volatile uint8_t dummy;
	uint8_t regDio2;
	
	//1.��������ʱ��
	//�˿����ã�A18 <- EXTAL ��Ϊʱ�ӡ����롿
    PORTA_PCR18 = PORT_PCR_MUX(1);//����ΪGPIO����
    GPIOA_PDDR &= (uint32_t)~(uint32_t)(1<<18);

    //2.��NVIC�п���D���жϣ�����D�ڸ���ΪGPIO���ܣ�����
    MKW01xDrv_IRQ_PortConfig();//����D���жϣ�D��ʱ�����ѿ����������ã�������ΪGPIO����
    //DIO1��DIO0��DIO4����������Ϊ�����롿�����ڽ��մ�RF��������Ϣ��������Ϣ��μ�DIO��Ϣӳ���
    GPIOC_PDDR &= (uint32_t)~(uint32_t)(1<<3);      //PTC3<-DIO0
    GPIOC_PDDR &= (uint32_t)~(uint32_t)(1<<4);      //PTC4<-DIO1
    GPIOD_PDDR &= (uint32_t)~(uint32_t)(1<<4);      //PTD4<-DIO4     

	//3.����D���ж�Ӧ��DIO0��DIO1��DIO4Ϊ�������ж�
//	PORTC_PCR3 |= PORT_PCR_ISF_MASK; //  д1���жϱ�־λ.
//	PORTC_PCR3 |= PORT_PCR_IRQC(0x09);  //�����ش����ж�
	PORTC_PCR4 |= PORT_PCR_ISF_MASK; //  д1���жϱ�־λ.
	PORTC_PCR4 |= PORT_PCR_IRQC(0x09);  //�����ش����ж�
//	PORTD_PCR4 |= PORT_PCR_ISF_MASK; // д1���жϱ�־λ.
//	PORTD_PCR4 |= PORT_PCR_IRQC(0x09);  //�����ش����ж�
	
	enable_irq(31);//��NVIC�п���C��D���ж�  
   
    //4.��ʼ��SPI��ʹ��SPIģ��0��SPI0Ĭ��ʹ������ʱ��
    //��ʼ��SPI0ģ��
    MKW01Drv_SPI0_Init();
    
	//5.ʹ���ⲿʱ�ӽ���SPI0��ȡ59�Ų���* Use external clock for testing */
	dummy = MKW01Drv_ReadRegister(0x59);
	dummy &= 0xEF;
	dummy |= 0x10;
	MKW01Drv_WriteRegister(0x59, dummy);  
	
	//6.SPI������ɺ��ٴθ�λ�������ߣ�������E30 �����λ���ţ��Ա��ڼ�⡢ȷ��CLKOUT�Ƿ�ɹ����룩
	//@AC RESET sequence from SX1233 datasheet:
	// RESET high-Z
	// RESET = 1, then wait 100us, RESET = 0 (high-Z), then wait 5ms before using the Radio. ע��high-z��Ϊ�ȸ��ٵ�ʱ��
	MKW01DRV_ASSERTRESET();//E��30�Ž���1
	Delay_us(100);                              //��ǰ��ʱ230uS? TODO, ��Ҫ���²���
	MKW01DRV_DEASSERTRESET();//E��30�Ž���0
	Delay_ms(5);                                //��ǰ��ʱ15.6mS? TODO, ��Ҫ���²���
	while((GPIOA_PDIR & (1<<18)) == 0); //�ȴ�CLKOUT���루���ȴ�A����������Ĵ�����18�Ž�λ����1��
	
	
	//7.����RF������CLKOUT������ΪMCUȡ��ʱ��Դ��32MHz��1��Ƶ����Ϊ32Mhz��//32Mhz/16= 2MHz CLKOUT
	regDio2 = MKW01Drv_ReadRegister(MKW01_Reg_DioMapping2);//��ȡCLKOUT��Ƶ��
	regDio2 = (uint8_t)(regDio2 & 0xF8);//���üĴ�����3Ϊ��0
	MKW01Drv_WriteRegister(MKW01_Reg_DioMapping2, (uint8_t) (regDio2 | 0x04)); //0x04����16��Ƶ//0x00����1��Ƶ
		
	//8.��ʼ������ʱ�����ں�ʱ�ӡ�
	MCG_Setup();//�õ�48MHz�ں�ʱ�ӣ�24MHz����ʱ��
	
	//9.����PA ramp��ֵ
	MKW01Drv_WriteRegister(MKW01_Reg_PaRamp, 0x0A);  //FSK����ģʽ�µ�PA�����½���б�ʡ����Σ�ʱ��Ϊ�� 31us
}



//��ʼ������ʱ�����ں�ʱ��
/******************************
Radio CLKOUT= 32MHz
PLL input= 2MHz (Prdiv=16)
MCG out= 48MHz
PLL out Clk= 24MHz
BUS_CLK= MCG out/2= 24 MHz
******************************/
void MCG_Setup(void) 
{
	int pll_val;  
	SIM_CLKDIV1 = ( 0
					| SIM_CLKDIV1_OUTDIV1(0)   //��pll������ʱ��Դ����1��Ƶ���õ��ں�ʱ�ӣ�48MHz��
					| SIM_CLKDIV1_OUTDIV4(1) );//2��Ƶ�ں�ʱ��Դ���õ����ߡ�flashʱ��24M     (48MHz)/2= 24MHz
					//| SIM_CLKDIV1_OUTDIV4(2) ); //3��Ƶ�ں�ʱ��Դ���õ����ߡ�falshʱ��16M  (48MHz)/3= 16MHz
   /* ��ʼ��PLL */
   /*PLL����MCG CLKOUT��ʱ��Դ������core, system, ��flash��ʱ�Ӷ������Դ����ֳ�ȥ*/ 
   pll_val= pll_init(2000000,//32000000,  /* CLKIN0Ƶ��*/
						 0,     /* ���õ͹�������ģʽ */
						 0,//1,     /* ѡ�������������ʱ������ */
						 1,//16,    /* PLLԤ��Ƶֵ */
						 24,    /* PLL��Ƶֵ */
						 1);    /* ʹ�ô�PLLʱ�������ΪMCGOUT */

   /*ȷ��PLL�Ƿ��ʼ���ɹ� */
   if (pll_val < 0x100)
	 while(1);
   
   SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;   // 0��MCGFLL_CLK clock   1��MCGPLLCLK clock with fixed divide by two
}

//��ʼ��PLL
int pll_init(int crystal_val, unsigned char hgo_val, unsigned char erefs_val, \
		     signed char prdiv_val, signed char vdiv_val, unsigned char mcgout_select)
{
  unsigned char frdiv_val;
  unsigned char temp_reg;
  unsigned char prdiv, vdiv;
  short i;
  int ref_freq;
  int pll_freq;

  //����Ƿ���FEIģʽ
  if (!((((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) && // check CLKS mux has selcted FLL output
      (MCG_S & MCG_S_IREFST_MASK) &&                                  // check FLL ref is internal ref clk
      (!(MCG_S & MCG_S_PLLST_MASK))))                                 // check PLLS mux has selected FLL
  {
    return 0x1;                                                     // return error code
  }

  // ����ⲿƵ��С�����Ƶ��
  if  (crystal_val > 50000000) {return 0x21;}

  // �����������PLL�ο�ֵ�����龧��Ƶ��ʱ���ڹ��֮��
  if (erefs_val)
  {
//    if ((crystal_val < 3000000) || (crystal_val > 32000000)) {return 0x22;} // return 1 if one of the available crystal options is not available
  }

  // make sure HGO will never be greater than 1. Could return an error instead if desired.
  if (hgo_val > 0)
  {
    hgo_val = 1; // force hgo_val to 1 if > 0
  }

  //���PLL��Ƶ�������Ƿ�Ϻ�����
  if ((prdiv_val < 1) || (prdiv_val > 25)) {return 0x41;}
  if ((vdiv_val < 24) || (vdiv_val > 50)) {return 0x42;}

  //���PLL�ο�ʱ��Ƶ���Ƿ�Ϻ�����
  ref_freq = crystal_val / prdiv_val;
  if ((ref_freq < 2000000) || (ref_freq > 4000000)) {return 0x43;}

  //���PLL���Ƶ��ʱ��Ϻ�����
  pll_freq = (crystal_val / prdiv_val) * vdiv_val;
  if ((pll_freq < 48000000) || (pll_freq > 100000000)) {return 0x45;}

  //����MCG_C1�Ĵ���
  // the RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
  // it still needs to be set correctly even if the oscillator is not being used
      
  temp_reg = MCG_C2;
  temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK); // clear fields before writing new values
    
  if (crystal_val <= 8000000)
  {
    temp_reg |= (MCG_C2_RANGE0(1) | (hgo_val << MCG_C2_HGO0_SHIFT) | (erefs_val << MCG_C2_EREFS0_SHIFT));
  }
  else
  {
    temp_reg |= (MCG_C2_RANGE0(2) | (hgo_val << MCG_C2_HGO0_SHIFT) | (erefs_val << MCG_C2_EREFS0_SHIFT));
  }
  MCG_C2 = temp_reg;
  
  // ȷ�����ڲο�ʱ�ӵ�FRDIVƵ��
  // since the external frequency has already been checked  
  // only the maximum frequency for each FRDIV value needs to be compared here.
  if (crystal_val <= 1250000) {frdiv_val = 0;}
  else if (crystal_val <= 2500000) {frdiv_val = 1;}
  else if (crystal_val <= 5000000) {frdiv_val = 2;}
  else if (crystal_val <= 10000000) {frdiv_val = 3;}
  else if (crystal_val <= 20000000) {frdiv_val = 4;}
  else {frdiv_val = 5;}

  // Select external oscillator and Reference Divider and clear IREFS to start ext osc
  // If IRCLK is required it must be enabled outside of this driver, existing state will be maintained
  // CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0
  temp_reg = MCG_C1;
  temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK); // Clear values in these fields
  temp_reg |= (MCG_C1_CLKS(2) | MCG_C1_FRDIV(frdiv_val)); // Set the required CLKS and FRDIV values
  MCG_C1 = temp_reg;

  // if the external oscillator is used need to wait for OSCINIT to set
  if (erefs_val)
  {
    for (i = 0 ; i < 20000 ; i++)             
    {
      if (MCG_S & MCG_S_OSCINIT0_MASK) break; // jump out early if OSCINIT sets before loop finishes
    }
  if (!(MCG_S & MCG_S_OSCINIT0_MASK)) return 0x23; // check bit is really set and return with error if not set
  }

  // �ȴ��ο�ʱ��״̬λ���
  for (i = 0 ; i < 2000 ; i++)
  {
    if (!(MCG_S & MCG_S_IREFST_MASK)) break; // jump out early if IREFST clears before loop finishes
  }
  if (MCG_S & MCG_S_IREFST_MASK) return 0x11; // check bit is really clear and return with error if not set

  // �ȴ�ʱ��״̬λ��ʾʱ��ԴΪ�ⲿ�ο�ʱ��
  for (i = 0 ; i < 2000 ; i++)
  {
    if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break; // jump out early if CLKST shows EXT CLK slected before loop finishes
  }
  if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2) return 0x1A; // check EXT CLK is really selected and return with error if not

  // Now in FBE
  // It is recommended that the clock monitor is enabled when using an external clock as the clock source/reference.
  // It is enabled here but can be removed if this is not required.
  MCG_C6 |= MCG_C6_CME0_MASK;
  
  // Configure PLL
  // Configure MCG_C5
  // If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed in here or in user code.
  temp_reg = MCG_C5;
  temp_reg &= ~MCG_C5_PRDIV0_MASK;
  temp_reg |= MCG_C5_PRDIV0(prdiv_val - 1);    //set PLL ref divider
  MCG_C5 = temp_reg;

  // Configure MCG_C6
  // The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
  // The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6
  temp_reg = MCG_C6; // store present C6 value
  temp_reg &= ~MCG_C6_VDIV0_MASK; // clear VDIV settings
  temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv_val - 24); // write new VDIV and enable PLL
  MCG_C6 = temp_reg; // update MCG_C6

  // wait for PLLST status bit to set
  for (i = 0 ; i < 2000 ; i++)
  {
    if (MCG_S & MCG_S_PLLST_MASK) break; // jump out early if PLLST sets before loop finishes
  }
  if (!(MCG_S & MCG_S_PLLST_MASK)) return 0x16; // check bit is really set and return with error if not set

  // Wait for LOCK bit to set
  for (i = 0 ; i < 4000 ; i++)
  {
    if (MCG_S & MCG_S_LOCK0_MASK) break; // jump out early if LOCK sets before loop finishes
  }
  if (!(MCG_S & MCG_S_LOCK0_MASK)) return 0x44; // check bit is really set and return with error if not set

  // Use actual PLL settings to calculate PLL frequency
  prdiv = ((MCG_C5 & MCG_C5_PRDIV0_MASK) + 1);
  vdiv = ((MCG_C6 & MCG_C6_VDIV0_MASK) + 24);

  // now in PBE

  MCG_C1 &= ~MCG_C1_CLKS_MASK; // clear CLKS to switch CLKS mux to select PLL as MCG_OUT

  // Wait for clock status bits to update
  for (i = 0 ; i < 2000 ; i++)
  {
    if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3) break; // jump out early if CLKST = 3 before loop finishes
  }
  if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3) return 0x1B; // check CLKST is set correctly and return with error if not

  // Now in PEE
  
  return ((crystal_val / prdiv) * vdiv); //MCGOUT equals PLL output frequency
} // pll_init
