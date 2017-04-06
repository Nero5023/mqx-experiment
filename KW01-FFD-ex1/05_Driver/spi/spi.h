//============================================================================
//�ļ����ƣ�spi.h
//���ܸ�Ҫ��KW01 spi�ײ���������ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//��    ����V3.0(WYH),20140909
//============================================================================
#ifndef _SPI_H
#define _SPI_H

#include "common.h"
#include "includes.h"
/************************************************************************************
* MCU SPI module configuration constants
************************************************************************************/
// SPI Control Register 1
//                            .--------- SPIE     : SPI Interrupt (SPRF and MODF) disabled
//                            |.-------- SPE      : SPI System inactive
//                            ||.------- SPTIE    : SPI Transmit Interrupt disabled
//                            |||.------ MSTR     : Master/Slave Mode Select.
//                            ||||.----- CPOL     : Clock Polarity. Active High
//                            |||||.---- CPHA     : Clock Phase. First edge on SPSCK occurs at the middle of the first cycle of an 8-cycle data transfer
//                            ||||||.--- SSOE     : Slave Select Output Enable
//                            |||||||.-- LSBFE    : LSB First (Shifter Direction). SPI serial data transfers start with most significant bit
//                            ||||||||
#define CSPIC1_INIT  0x00 //0b00010000
// SPI Control Register 2
//                            .--------- SPMIE       : SPI match interrupt enable
//                            |.-------- SPIMODE     : SPI 8-bit or 16-bit mode                                         
//                            ||.------- TXDMAE      : Transmit DMA Enable
//                            |||.------ MODFEN      : Master Mode-Fault Function disabled.
//                            ||||.----- BIDIROE     : Bidirectional Mode Output Enable disabled
//                            |||||.---- RXDMAE      : Receive DMA enable
//                            ||||||.--- SPISWAI     : SPI Stop in Wait Mode. SPI clocks continue to operate in wait mode
//                            |||||||.-- SPC0        : SPI Pin Control 0. SPI uses separate pins for data input and data output.
//                            ||||||||
#define CSPIC2_INIT  0x00 //0b00000000
// SPI Baud Rate Register. Master SPI CLOCK = BUS CLOCK / Prescaler Divisor / SPI Rate Divisor = BUS CLOCK / 2
//                            .--------- X        : Unimplemented or Reserved
//                            |...------ SPPR[2:0]: SPI Baud Rate Prescale Divisor. Prescaler Divisor = 1
//                            ||||....-- SPR[3:0] : SPI Baud Rate Divisor. SPI Rate Divisor = 2
//                            ||||||||
#define CSPIBR_INIT   0x00   //0b001110010    /                                 /Original line, SPICLK=BUS_CLK/2 (2: baud rate divisor)
//#define cSPIBR_Init   0x01                                                    //modif->  SPICLK=BUS_CLK/4 (4: baud rate divisor)
//#define cSPIBR_Init   0x02                                                    //modif->  SPICLK=BUS_CLK/8 (8: baud rate divisor)
//#define cSPIBR_Init   0x04                                                    //modif->  SPICLK=BUS_CLK/32 (32: baud rate divisor)
//#define cSPIBR_Init   0x08                                                    //modif->  SPICLK=BUS_CLK/512 (512: baud rate divisor) 

/* SPI Status Register masks */


//RFģ����أ�SPIͨ�����ã�
#define MKW01DRV_DEASSERTRESET()    { GPIOE_PCOR |= (1<<30); }      //д1���Ӧ��PTE30����������Ĵ�����ʹPDORΪ0
#define MKW01DRV_ASSERTRESET()      { GPIOE_PDOR |= (1<<30);}       //�������Ϊ�߼�1

#define SPID_DUMMYREAD              {char dummy = SPI0_BASE_PTR->DL;}//{char dummy = SPI0_BASE_PTR->D;}
#define SPIS_DUMMYREAD              {char dummy = SPI0_BASE_PTR->S;}

//#define AssertSS                    {GPIOC_PCOR |= (uint_8)(1<<4);}//д1���Ӧ��PTC4����������Ĵ�����ʹPDORΪ0��������ͨ�ţ�
//#define DeAssertSS                  {GPIOC_PDOR |= (uint_8)(1<<4);}//�������Ϊ�߼�1(�ر�����ͨ��)

#define ASSERTSS                    {GPIOD_PCOR |= (uint_8)(1<<0);}//д1���Ӧ��PTD0����������Ĵ�����ʹPDORΪ0��������ͨ�ţ�
#define DEASSERTSS                  {GPIOD_PDOR |= (uint_8)(1<<0);}//�������Ϊ�߼�1(�ر�����ͨ��)

#define WAIT_SPITX_BUFFER_EMPTY     {while(!(SPI0_BASE_PTR->S & SPI_S_SPTEF_MASK));}
#define WAIT_SPIRX_BUFFER_FULL      {while(!(SPI0_BASE_PTR->S & SPI_S_SPRF_MASK));}
#define WAIT_SPI_TRANSMITTER_EMPTY  {WAIT_SPITX_BUFFER_EMPTY; Wait_6_BitClks();  }

//============================================================================
//�������ƣ�MKW01Drv_SPI0_Init
//�������أ���      
//����˵����addr������ģ��Ĵ�����ַ
//���ܸ�Ҫ����ȡ����ģ��Ĵ��� 
//============================================================================
void MKW01Drv_SPI0_Init();

//============================================================================
//�������ƣ�MKW01Drv_ReadRegister
//�������أ�val      
//����˵����addr������ģ��Ĵ�����ַ
//���ܸ�Ҫ����ȡ����ģ��Ĵ��� 
//============================================================================
uint_8 MKW01Drv_ReadRegister(uint_8 addr);

//============================================================================
//�������ƣ�MKW01Drv_WriteRegister
//�������أ���      
//����˵����addr������ģ��Ĵ�����ַ��val����д���ֵ
//���ܸ�Ҫ����������ģ��Ĵ��� 
//============================================================================
void MKW01Drv_WriteRegister(uint_8 addr, uint_8 val);

//============================================================================
//�������ƣ�MKW01Drv_WriteRegisterFast
//�������أ�����ȫ�ĺ���,ֻ���ڽ�ֹ�ж�ʱ�ſ�ʹ��
//����˵����addr������ģ��Ĵ�����ַ��val����д���ֵ
//���ܸ�Ҫ����������ģ��Ĵ��� 
//============================================================================
void MKW01Drv_WriteRegisterFast(uint_8 addr, uint_8 val);


#define MKW01Drv_WriteRegisterFastMacro(addr, val)\
  SPIS_DUMMYREAD;\
  ASSERTSS;\
  SPI0_BASE_PTR->DL = (unsigned char)(addr | 0x80);\
  WAIT_SPITX_BUFFER_EMPTY;\
  SPID_DUMMYREAD;\
  SPI0_BASE_PTR->DL = (unsigned char)(val);\
  WAIT_SPITX_BUFFER_EMPTY;\
  SPID_DUMMYREAD;\
  WAIT_SPIRX_BUFFER_FULL;\
  DEASSERTSS;\
    

#define MKW01Drv_ReadRegisterFastMacro(addr, val)\
  SPIS_DUMMYREAD;\
  ASSERTSS;\
  SPI0_BASE_PTR->DL = (unsigned char)(addr);\
  WAIT_SPITX_BUFFER_EMPTY;\
  SPID_DUMMYREAD;\
  SPI0_BASE_PTR->DL = 0x00;\
  WAIT_SPITX_BUFFER_EMPTY;\
  SPID_DUMMYREAD;\
  WAIT_SPIRX_BUFFER_FULL;\
  (val) = SPI0_BASE_PTR->DL;\
  DEASSERTSS;\

#endif /* _SPI_H */
