
#include "common.h"
#include "sysinit.h"
#include "rf.h"
#include "spi.h"

#define	MKW01_Reg_DioMapping2	  	0x26//Mapping	of	pins	DIO4	and	DIO5,	ClkOut	frequency		0x05	0x07
#define	MKW01_Reg_PaRamp	        0x12//	Control	of	the	PA	ramp	time	in	FSK	mode	0x09		

extern int main(void);
//声明内部函数
static void vector_init(void);
static void m_zero_fill_bss(void);
static void m_data_seg_init(void);
void sys_pin_enable_port(void);



void Platform_Init(void);
void MCG_Setup(void);
int pll_init(int crystal_val, unsigned char hgo_val,\
			unsigned char erefs_val, signed char prdiv_val,\
			signed char vdiv_val, unsigned char mcgout_select);

//看门狗WDOG(COP)宏定义
#define WDOG_DISABLE() {SIM_COPC = 0x00u;}  //禁用看门狗
//COP使用LPO时钟，COP在2^10个LPO时钟周期后延时
#define WDOG_ENABLE() {SIM_COPC |= SIM_COPC_COPT_MASK;} //使能看门狗

void startup(void)
{

	WDOG_DISABLE(); //关看门狗
	
//	vector_init();          //复制中断向量表至RAM【GZF 2014-4-23】
	m_zero_fill_bss();      //清bss段
	m_data_seg_init();      //将ROM中的初始化数据拷贝到RAM中
//	sys_init();
	sys_pin_enable_port();  //使能所有端口时钟，打开对应端口的时钟门，以便于配置引脚复用功能
	
	DISABLE_INTERRUPTS;     //关中断，进入临界区
	
	Platform_Init();        //初始化MCU及RF
	
	ENABLE_INTERRUPTS;      //开中断，退出临界区

	main();
}

//拷贝中断向量表
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

// 初始化BSS段
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
	
	//1.设置输入时钟
	//端口设置，A18 <- EXTAL 作为时钟【输入】
    PORTA_PCR18 = PORT_PCR_MUX(1);//复用为GPIO功能
    GPIOA_PDDR &= (uint32_t)~(uint32_t)(1<<18);

    //2.在NVIC中开启D口中断，并将D口复用为GPIO功能，输入
    MKW01xDrv_IRQ_PortConfig();//开启D口中断（D口时钟门已开，可以配置），复用为GPIO功能
    //DIO1，DIO0，DIO4三个引脚作为【输入】，便于接收从RF过来的消息，具体信息请参见DIO信息映射表
    GPIOC_PDDR &= (uint32_t)~(uint32_t)(1<<3);      //PTC3<-DIO0
    GPIOC_PDDR &= (uint32_t)~(uint32_t)(1<<4);      //PTC4<-DIO1
    GPIOD_PDDR &= (uint32_t)~(uint32_t)(1<<4);      //PTD4<-DIO4     

	//3.设置D口中对应的DIO0、DIO1、DIO4为上升沿中断
//	PORTC_PCR3 |= PORT_PCR_ISF_MASK; //  写1清中断标志位.
//	PORTC_PCR3 |= PORT_PCR_IRQC(0x09);  //上升沿触发中断
	PORTC_PCR4 |= PORT_PCR_ISF_MASK; //  写1清中断标志位.
	PORTC_PCR4 |= PORT_PCR_IRQC(0x09);  //上升沿触发中断
//	PORTD_PCR4 |= PORT_PCR_ISF_MASK; // 写1清中断标志位.
//	PORTD_PCR4 |= PORT_PCR_IRQC(0x09);  //上升沿触发中断
	
	enable_irq(31);//在NVIC中开启C、D口中断  
   
    //4.初始化SPI，使用SPI模块0，SPI0默认使用总线时钟
    //初始化SPI0模块
    MKW01Drv_SPI0_Init();
    
	//5.使用外部时钟进行SPI0读取59号测试* Use external clock for testing */
	dummy = MKW01Drv_ReadRegister(0x59);
	dummy &= 0xEF;
	dummy |= 0x10;
	MKW01Drv_WriteRegister(0x59, dummy);  
	
	//6.SPI配置完成后，再次复位（先拉高，再拉低E30 这个复位引脚，以便于检测、确保CLKOUT是否成功输入）
	//@AC RESET sequence from SX1233 datasheet:
	// RESET high-Z
	// RESET = 1, then wait 100us, RESET = 0 (high-Z), then wait 5ms before using the Radio. 注：high-z即为先高再低时序
	MKW01DRV_ASSERTRESET();//E口30号脚置1
	Delay_us(100);                              //当前延时230uS? TODO, 需要重新测量
	MKW01DRV_DEASSERTRESET();//E口30号脚清0
	Delay_ms(5);                                //当前延时15.6mS? TODO, 需要重新测量
	while((GPIOA_PDIR & (1<<18)) == 0); //等待CLKOUT输入（即等待A口数据输入寄存器的18号脚位被置1）
	
	
	//7.配置RF出来的CLKOUT，进而为MCU取得时钟源（32MHz，1分频，仍为32Mhz）//32Mhz/16= 2MHz CLKOUT
	regDio2 = MKW01Drv_ReadRegister(MKW01_Reg_DioMapping2);//读取CLKOUT的频率
	regDio2 = (uint8_t)(regDio2 & 0xF8);//将该寄存器低3为清0
	MKW01Drv_WriteRegister(MKW01_Reg_DioMapping2, (uint8_t) (regDio2 | 0x04)); //0x04代表16分频//0x00代表1分频
		
	//8.初始化总线时钟与内核时钟。
	MCG_Setup();//得到48MHz内核时钟，24MHz总线时钟
	
	//9.设置PA ramp的值
	MKW01Drv_WriteRegister(MKW01_Reg_PaRamp, 0x0A);  //FSK调制模式下的PA上升下降（斜率、整形）时间为： 31us
}



//初始化总线时钟与内核时钟
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
					| SIM_CLKDIV1_OUTDIV1(0)   //对pll产生的时钟源进行1分频，得到内核时钟（48MHz）
					| SIM_CLKDIV1_OUTDIV4(1) );//2分频内核时钟源，得到总线、flash时钟24M     (48MHz)/2= 24MHz
					//| SIM_CLKDIV1_OUTDIV4(2) ); //3分频内核时钟源，得到总线、falsh时钟16M  (48MHz)/3= 16MHz
   /* 初始化PLL */
   /*PLL将是MCG CLKOUT的时钟源，所以core, system, 和flash的时钟都可以以此来分出去*/ 
   pll_val= pll_init(2000000,//32000000,  /* CLKIN0频率*/
						 0,     /* 设置低功耗振荡器模式 */
						 0,//1,     /* 选择晶振或内置振荡器时钟输入 */
						 1,//16,    /* PLL预分频值 */
						 24,    /* PLL倍频值 */
						 1);    /* 使用此PLL时钟输出作为MCGOUT */

   /*确定PLL是否初始化成功 */
   if (pll_val < 0x100)
	 while(1);
   
   SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK;   // 0：MCGFLL_CLK clock   1：MCGPLLCLK clock with fixed divide by two
}

//初始化PLL
int pll_init(int crystal_val, unsigned char hgo_val, unsigned char erefs_val, \
		     signed char prdiv_val, signed char vdiv_val, unsigned char mcgout_select)
{
  unsigned char frdiv_val;
  unsigned char temp_reg;
  unsigned char prdiv, vdiv;
  short i;
  int ref_freq;
  int pll_freq;

  //检测是否处于FEI模式
  if (!((((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) && // check CLKS mux has selcted FLL output
      (MCG_S & MCG_S_IREFST_MASK) &&                                  // check FLL ref is internal ref clk
      (!(MCG_S & MCG_S_PLLST_MASK))))                                 // check PLLS mux has selected FLL
  {
    return 0x1;                                                     // return error code
  }

  // 检查外部频率小于最大频率
  if  (crystal_val > 50000000) {return 0x21;}

  // 如果晶振被用作PLL参考值，则检查晶振频率时候处于规格之内
  if (erefs_val)
  {
//    if ((crystal_val < 3000000) || (crystal_val > 32000000)) {return 0x22;} // return 1 if one of the available crystal options is not available
  }

  // make sure HGO will never be greater than 1. Could return an error instead if desired.
  if (hgo_val > 0)
  {
    hgo_val = 1; // force hgo_val to 1 if > 0
  }

  //检测PLL分频器设置是否合乎规则
  if ((prdiv_val < 1) || (prdiv_val > 25)) {return 0x41;}
  if ((vdiv_val < 24) || (vdiv_val > 50)) {return 0x42;}

  //检测PLL参考时钟频率是否合乎规则
  ref_freq = crystal_val / prdiv_val;
  if ((ref_freq < 2000000) || (ref_freq > 4000000)) {return 0x43;}

  //检测PLL输出频率时候合乎规则
  pll_freq = (crystal_val / prdiv_val) * vdiv_val;
  if ((pll_freq < 48000000) || (pll_freq > 100000000)) {return 0x45;}

  //配置MCG_C1寄存器
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
  
  // 确定基于参考时钟的FRDIV频率
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

  // 等待参考时钟状态位清除
  for (i = 0 ; i < 2000 ; i++)
  {
    if (!(MCG_S & MCG_S_IREFST_MASK)) break; // jump out early if IREFST clears before loop finishes
  }
  if (MCG_S & MCG_S_IREFST_MASK) return 0x11; // check bit is really clear and return with error if not set

  // 等待时钟状态位显示时钟源为外部参考时钟
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
