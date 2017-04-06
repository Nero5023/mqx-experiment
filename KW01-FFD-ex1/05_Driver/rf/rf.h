//============================================================================
//文件名称：rf.h
//功能概要：KW01 rf底层驱动程序头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版          本：V1.0 20130910,V3.2(WYH),20150103,V3.3(GZF),20150118
//============================================================================
#ifndef _RF_H
#define _RF_H

#include <stdlib.h>  //使用到标准数学库（例如随机函数）
#include "spi.h"     //MCU使用SPI与RF模块通信


//=========================================================================
//函数名称：RF_Init（RF模块初始化）
//功能概要：向RF模块寄存器写入初值
//参数说明：无
//函数返回：无
//=========================================================================
void RF_Init(uint_8 hardware_addr);   //RF模块初始化

//=========================================================================
//函数名称：RF_ReceiveFrame（RF模块接收一帧）
//功能概要：RF模块接收一帧
//参数说明：pbuf：接收数据存放的数组;plen:接收长度
//函数返回：接收状态标志位（=0,接收正常，=其他值，接收异常）
//=========================================================================
uint_8 RF_ReceiveFrame(uint_8* pbuf, uint_8* plen,uint_8 HW_ADR);

//=========================================================================
//函数名称：RFEnergyDetect(获得接收能量值)
//功能概要：读取RSSI
//参数说明：无
//函数返回：注意，返回的RSSI值为其绝对值，用户在使用时应取负数，如函数返回5，其实际RSSI值为-5
//=========================================================================
uint_8 RFEnergyDetect(void);  //获得接收能量值绝对值0~110

//=========================================================================
//函数名称：RFTempDetect
//功能概要：读取rf温度值
//参数说明：无
//函数返回：无
//=========================================================================
uint_8 RFTempDetect(void);
//=========================================================================
//函数名称：RF_SendData
//功能概要：RF模块发送函数
//参数说明：
//函数返回：无
//=========================================================================
void RF_SendData(uint_8 dataLength,uint_8* data,uint_8 HW_ADR);

//===========================================================================
//函数名称：MKW01Drv_IRQ_DIO1_Enable
//功能概要：使能IRQ1
//参数说明：无
//函数返回：无
//===========================================================================
void MKW01Drv_IRQ_DIO1_Enable(void);

//============================================================================
//函数名：RFSendDataByCSMACA
//功能：CSMA/CA算法：
//      首先延时一个随机(2^BE范围内 BE=minBE)的时间，然后判断信道是否空闲，如果空闲发送，如果
//      不空闲随机延时更长的时间(BE++,在BE和maxBE之间延时)。可以循环(NB<maxNB)次,维持两个变量
//      BE和NB。BE初始化为minBE。每次延时都是在(2^BE-1)的范围内随机延时一个时间。若信道不空闲
//      ：BE=min(BE+1,maxBE)。NB为循环变量
//参数：data--要发送数据的首地址
//      len--发送的数据长度
//      pRTxMode--RF模式的设置
//      maxNB--CSMA/CA算法中：表示最多尝试发送的次数，如果最后还是发送失败了，那么会强制发送一次
//      maxBE--CSMA/CA算法中：表示退避指数的最大值
//返回：0---表示发送成功  1--表示失败
//============================================================================
void RFSendDataByCSMACA(uint_8 DataLen,uint_8 *DataAdr, uint_8 Energy,uint_8 HW_ADR);

//RF初始化设置结构体
typedef struct  RF_InitAttr{
    //1 硬件过滤地址设置
	//  取值范围(0~255)
    uint_8 HardWareAddress;
    //2 载波频率设置
    //取值范围：Frf=433M±333K(Hz)
    //计算方法：
    //	晶振频率：Fxosc=32M
    //	频率分辨率：Fstep=Fxosc/2(19)=32M/2(19)=61.035Hz
    //	载波频率：Frf=Fstep*(MSB,Mid,Lsb)=61.035*0x6c4000=433M
    uint_8 CarrierFrequencyMsb;   //通信频段
    uint_8 CarrierFrequencyMid;   //通信频段
    uint_8 CarrierFrequencyLsb;   //通信频段
    //3 无线低噪声放大增益
    //此处设置表示接收时，实际增益=（最大增益-12）；，
    //取值范围
    //自动增益：Lna_LnaGain_Agc
    //手动增益: Lna_LnaGain_MaxGain
    //         Lna_LnaGain_MaxGain_6
    //         Lna_LnaGain_MaxGain_12
    //         Lna_LnaGain_MaxGain_24
    //         Lna_LnaGain_MaxGain_48(通信距离最短)
    //当接收距离过远时，推荐使用自动增益：Lna_LnaGain_Agc
    //当接收距离较近时，推荐使用手动增益：Lna_LnaGain_MaxGain_12
    uint_8 LowNoiseAmplifierGain;
    //4 接收信号强度阈值寄存器RssiThresh
    //取值范围：0~255
    //接收信号强度阈值=-RssiThresh/2
    //阈值范围：0~-128,推荐值为-90~-100,
    //即RssiThresh寄存器推荐值为180~200
    //若接收信号强度阈值=-110,则RssiThresh=220=0xDC
    uint_8 RssiThresh;
    //5 RF输出功率寄存器PaLevel[0-4]设置
    //取值范围：0~0x1F
    //输出功率=-18+PaLevel[0-4]dBm
    //输出功率范围：-18~13dBm
    //默认推荐按最大功率发送，低功耗应用时根据需要设置
    uint_8 OutPutPower;
}volatile RF_InitAttribute,*RF_InitAttributePtr;

//=========================================================================
//函数名称：RF_InitSet
//功能概要：RF初始化参数设置
//参数说明：
//        1 硬件过滤地址设置，取值范围(0~255)
//          rf_InitAttribute.HardWareAddress
//        2 载波频率设置
//          rf_InitAttribute.FrequencyDeviationMsb;   //通信频段
//          rf_InitAttribute.FrequencyDeviationMid;   //通信频段
//          rf_InitAttribute.FrequencyDeviationLsb;   //通信频段
//          取值范围：Frf=433M±333K(Hz)
//                        Msb Mid Lsb
//          433.333MHz=>0x6C  55  62
//          433MHz    =>0x6C  40  00
//          432.667MHz=>0x6C  2A  C2
//        3 无线低噪声放大增益
//	        rf_InitAttribute.LowNoiseAmplifierGain
//          自动增益：Lna_LnaGain_Agc
//          手动增益: Lna_LnaGain_MaxGain
//          Lna_LnaGain_MaxGain_6
//          Lna_LnaGain_MaxGain_12
//          Lna_LnaGain_MaxGain_24
//          Lna_LnaGain_MaxGain_48(通信距离最短)
//          当接收距离过远时，推荐使用自动增益：Lna_LnaGain_Agc
//          当接收距离较近时，推荐使用手动增益：Lna_LnaGain_MaxGain_12
//        4 接收信号强度阈值寄存器RssiThresh，取值范围：0~255
//          rf_InitAttribute.RssiThresh
//        5 RF输出功率寄存器PaLevel[0-4]设置，取值范围：0~0x1F
//          rf_InitAttribute.OutPutPower
//函数返回：无
//=========================================================================
void RF_InitSet(RF_InitAttributePtr rf_InitAttribute);

//------------------------------------------------------------------------*
//函数名:RFChannelReq
//功  能:查询通道号
//参  数:无
//返  回:通道号
//说  明:无
//------------------------------------------------------------------------*
uint_8 RFChannelReq(void);


//#define HW_ADR    0x53  	   //硬件地址过滤，可设0x01~0xFE?
#define PAY_LOAD_LENGTH  64    //RF硬件发送能力（地址+数据包长度+数据包）


//错误码
#define ERR_RF_NONE     0
#define ERR_RF_NULL     1
#define ERR_RF_SEND     2
#define ERR_RF_RECV     3
#define ERR_RF_RDRAM    4

//#define ENERGY_SEND_MIN 0xA5
//能量检测阈值（用于冲突避免发送功能中），当能量绝对大于此值时，那么进行发送
#define ENERGY_SEND_MIN 0x58
#define RFCH_MAX        0xF

//============================================================================
//功能概要：KW01 rf底层寄存器映射头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2014-3-17  V1.0   初始版本
//============================================================================
//    FIFO read/write access                                          0x00  //
#define    MKW01_Reg_Fifo              0x00
//    Operating modes of the transceiver                              0x04  //
#define    MKW01_Reg_OpMode            0x01
//    Data operation mode and Modulation settings                     0x00  //
#define    MKW01_Reg_DataModul         0x02
//    Bit Rate setting, Most Significant Bits                         0x1A  //
#define    MKW01_Reg_BitrateMsb        0x03
//    Bit Rate setting, Least Significant Bits                        0x0B  //
#define    MKW01_Reg_BitrateLsb        0x04
//    Frequency Deviation setting, Most Significant Bits              0x00  //
#define    MKW01_Reg_FdevMsb           0x05
//    Frequency Deviation setting, Least Significant Bits             0x52  //
#define    MKW01_Reg_FdevLsb           0x06
//    RF Carrier Frequency, Most Significant Bits                     0xE4  //
#define    MKW01_Reg_FrfMsb            0x07
//    RF Carrier Frequency, Intermediate Bits                         0xC0  //
#define    MKW01_Reg_FrfMid            0x08
//    RF Carrier Frequency, Least Significant Bits                    0x00  //
#define    MKW01_Reg_FrfLsb            0x09
//    RC Oscillators Settings                                         0x41  //
#define    MKW01_Reg_Osc1              0x0A
//    AFC control in low modulation index situations                  0x00  //
#define    MKW01_Reg_AfcCtrl           0x0B
//    Low Battery Indicator Settings                                  0x02  //
#define    MKW01_Reg_LowBat            0x0C
//    Listen Mode settings                                            0x92  //
#define    MKW01_Reg_Listen1           0x0D
//    Listen Mode Idle duration                                       0xF5  //
#define    MKW01_Reg_Listen2           0x0E
//    Listen Mode Rx duration                                         0x20  //
#define    MKW01_Reg_Listen3           0x0F
//    Semtech ID relating the silicon revision                        0x22  //
#define    MKW01_Reg_Version           0x10
//    PA selection and Output Power control                           0x9F  //
#define    MKW01_Reg_PaLevel           0x11
//    Control of the PA ramp time in FSK mode                         0x09  //
#define    MKW01_Reg_PaRamp            0x12
//    Over Current Protection control                                 0x1A  //
#define    MKW01_Reg_Ocp               0x13
//    -                                                               0x40  //
#define    MKW01_Reg_Reserved14        0x14
//    -                                                               0xB0  //
#define    MKW01_Reg_Reserved15        0x15
//    -                                                               0x7B  //
#define    MKW01_Reg_Reserved16        0x16
//    -                                                               0x9B  //
#define    MKW01_Reg_Reserved17        0x17
//    LNA settings  0x08                                              0x88  //
#define    MKW01_Reg_Lna               0x18
//    Channel Filter BW Control   0x86                                0x55  //
#define    MKW01_Reg_RxBw              0x19
//    Channel Filter BW control during the AFC routine   0x8A         0x8B  //
#define    MKW01_Reg_AfcBw             0x1A
//    OOK demodulator selection and control in peak mode              0x40  //
#define    MKW01_Reg_OokPeak           0x1B
//    Average threshold control of the OOK demodulator                0x80  //
#define    MKW01_Reg_OokAvg            0x1C
//    Fixed threshold control of the OOK demodulator                  0x06  //
#define    MKW01_Reg_OokFix            0x1D
//    AFC and FEI control and status                                  0x10  //
#define    MKW01_Reg_AfcFei            0x1E
//    MSB of the frequency correction of the AFC                      0x00  //
#define    MKW01_Reg_AfcMsb            0x1F
//    LSB of the frequency correction of the AFC                      0x00  //
#define    MKW01_Reg_AfcLsb            0x20
//    MSB of the calculated frequency error                           0x00  //
#define    MKW01_Reg_FeiMsb            0x21
//    LSB of the calculated frequency error                           0x00  //
#define    MKW01_Reg_FeiLsb            0x22
//    RSSI-related settings                                           0x02  //
#define    MKW01_Reg_RssiConfig        0x23
//    RSSI value in dBm                                               0xFF  //
#define    MKW01_Reg_RssiValue         0x24
//    Mapping of pins DIO0 to DIO3                                    0x00  //
#define    MKW01_Reg_DioMapping1       0x25
//    Mapping of pins DIO4 and DIO5, ClkOut frequency  0x05           0x07  //
#define    MKW01_Reg_DioMapping2       0x26
//    Status register: PLL Lock state, Timeout, RSSI > Threshold...   0x80  //
#define    MKW01_Reg_IrqFlags1         0x27
//    Status register: FIFO handling flags, Low Battery detection...  0x00  //
#define    MKW01_Reg_IrqFlags2         0x28
//    RSSI Threshold control 0xFF                                     0xE4  //
#define    MKW01_Reg_RssiThresh        0x29
//    Timeout duration between Rx request and RSSI detection          0x00  //
#define    MKW01_Reg_RxTimeout1        0x2A
//    Timeout duration between RSSI detection and PayloadReady        0x00  //
 #define    MKW01_Reg_RxTimeout2        0x2B
//    Preamble length, MSB                                            0x00  //
#define    MKW01_Reg_PreambleMsb       0x2C
//    Preamble length, LSB                                            0x03  //
#define    MKW01_Reg_PreambleLsb       0x2D
//    Sync Word Recognition control                                   0x98  //
#define    MKW01_Reg_SyncConfig        0x2E
//    Sync Word bytes, 1 through 8  0x00                              0x01  //
#define    MKW01_Reg_SyncValue1        0x2F
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue2        0x30
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue3        0x31
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue4        0x32
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue5        0x33
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue6        0x34
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue7        0x35
//                                                                    0x00  //
#define    MKW01_Reg_SyncValue8        0x36
//    Packet mode settings                                            0x10  //
#define    MKW01_Reg_PacketConfig1     0x37
//    Payload length setting                                          0x40  //
#define    MKW01_Reg_PayloadLength     0x38
//    Node address                                                    0x00  //
#define    MKW01_Reg_NodeAdrs          0x39
//    Broadcast address                                               0x00  //
#define    MKW01_Reg_BroadcastAdrs     0x3A
//    Auto modes settings                                             0x00  //
#define    MKW01_Reg_AutoModes         0x3B
//    Fifo threshold, Tx start condition  0x0F                        0x8F  //
#define    MKW01_Reg_FifoThresh        0x3C
//    Packet mode settings                                            0x02  //
#define    MKW01_Reg_PacketConfig2     0x3D
//    16 bytes of the cypher key                                      0x00  //
#define    MKW01_Reg_AesKey1           0x3E
//                                                                    0x00  //
#define    MKW01_Reg_AesKey2           0x3F
//                                                                    0x00  //
#define    MKW01_Reg_AesKey3           0x40
//                                                                    0x00  //
#define    MKW01_Reg_AesKey4           0x41
//                                                                    0x00  //
#define    MKW01_Reg_AesKey5           0x42
//                                                                    0x00  //
#define    MKW01_Reg_AesKey6           0x43
//                                                                    0x00  //
#define    MKW01_Reg_AesKey7           0x44
//                                                                    0x00  //
#define    MKW01_Reg_AesKey8           0x45
//                                                                    0x00  //
#define    MKW01_Reg_AesKey9           0x46
//                                                                    0x00  //
#define    MKW01_Reg_AesKey10          0x47
//                                                                    0x00  //
#define    MKW01_Reg_AesKey11          0x48
//                                                                    0x00  //
#define    MKW01_Reg_AesKey12          0x49
//                                                                    0x00  //
#define    MKW01_Reg_AesKey13          0x4A
//                                                                    0x00  //
#define    MKW01_Reg_AesKey14          0x4B
//                                                                    0x00  //
#define    MKW01_Reg_AesKey15          0x4C
//                                                                    0x00  //
#define    MKW01_Reg_AesKey16          0x4D
//    Temperature Sensor control                                      0x01  //
#define    MKW01_Reg_Temp1             0x4E
//    Temperature readout                                             0x00  //
#define    MKW01_Reg_Temp2             0x4F
//    Sensitivity boost                                               0x1B  //
#define    MKW01_Reg_TestLna           0x58
//    AFC offset for low modulation index AFC                         0x00  //
#define    MKW01_Reg_TestAfc           0x71
//    Used in calibration procedure for V2a chip version                    //
#define    MKW01_Reg_Calib             0x57
//    Fading margin improvement. Added for extended PHY.                    //
#define    MKW01_Reg_TestDagc          0x6F
// RegDioMapping1 (0x25) //
// |7  6|5  4|3  2|1  0| //
// |DIO0|DIO1|DIO2|DIO3| //

// RegDioMapping2 (0x26) //
// |7  6|5  4|3 |2    0| //
// |DIO4|DIO5|0 |ClkOut| //

#define ClkOutFxOsc_Div1        (0x00)
#define ClkOutFxOsc_Div2        (0x01)
#define ClkOutFxOsc_Div4        (0x02)
#define ClkOutFxOsc_Div8        (0x03)
#define ClkOutFxOsc_Div16       (0x04)
#define ClkOutFxOsc_Div32       (0x05)
#define ClkOutRC                (0x06)
#define ClkOutOff               (0x07)

#define DIO0_RxCrkOk          (0x00 << 6)
#define DIO0_RxPayloadReady   (0x01 << 6)
#define DIO0_RxSyncAddress    (0x02 << 6)
#define DIO0_RxRssi           (0x03 << 6)

#define DIO1_RxFifoLevel      (0x00 << 4)
#define DIO1_RxFifoFull       (0x01 << 4)
#define DIO1_RxFifoNotEmpty   (0x02 << 4)
#define DIO1_RxTimeout        (0x03 << 4)

#define DIO2_RxFifoNotEmpty   (0x00 << 2)
#define DIO2_RxData           (0x01 << 2)
#define DIO2_RxLowBat         (0x02 << 2)
#define DIO2_RxAutoMode       (0x03 << 2)

#define DIO3_RxFifoFull       (0x00 << 0)
#define DIO3_RxRssi           (0x01 << 0)
#define DIO3_RxSyncAddres     (0x02 << 0)
#define DIO3_RxPllLock        (0x03 << 0)

#define DIO4_RxTimeout        (0x00 << 6)
#define DIO4_RxRssi           (0x01 << 6)
#define DIO4_RxRxReady        (0x02 << 6)
#define DIO4_RxPllLock        (0x03 << 6)

#define DIO5_RxClkOut         (0x00 << 4)
#define DIO5_RxData           (0x01 << 4)
#define DIO5_RxLowBat         (0x02 << 4)
#define DIO5_RxModeReady      (0x03 << 4)


#define DIO0_TxPacketSent     (0x00 << 6)
#define DIO0_TxTxReady        (0x01 << 6)
#define DIO0_TxLowBat         (0x02 << 6)
#define DIO0_TxPllLock        (0x03 << 6)

#define DIO1_TxFifoLevel      (0x00 << 4)
#define DIO1_TxFifoFull       (0x01 << 4)
#define DIO1_TxFifoNotEmpty   (0x02 << 4)
#define DIO1_TxTimeout        (0x03 << 4)

#define DIO2_TxFifoNotEmpty   (0x00 << 2)
#define DIO2_TxData           (0x01 << 2)
#define DIO2_TxLowBat         (0x02 << 2)
#define DIO2_TxAutoMode       (0x03 << 2)

#define DIO3_TxFifoFull       (0x00 << 0)
#define DIO3_TxTxReady        (0x01 << 0)
#define DIO3_TxLowBat         (0x02 << 0)
#define DIO3_TxPllLock        (0x03 << 0)

#define DIO4_TxModeReady      (0x00 << 6)
#define DIO4_TxTxReady        (0x01 << 6)
#define DIO4_TxLowBat         (0x02 << 6)
#define DIO4_TxPllLock        (0x03 << 6)

#define DIO5_TxClkOut         (0x00 << 4)
#define DIO5_TxData           (0x01 << 4)
#define DIO5_TxLowBat         (0x02 << 4)
#define DIO5_TxModeReady      (0x03 << 4)

// OpMode //
#define OpMode_Sequencer_Off  (0x01 << 7)
#define OpMode_Sequencer_On   (0x00 << 7)

#define OpMode_Listen_Off     (0x00 << 6)
#define OpMode_Listen_On      (0x01 << 6)
#define OpMode_Listen_Abort   (0x01 << 5)

#define OpMode_Sleep          (0x00 << 2)
#define OpMode_StandBy        (0x01 << 2)
#define OpMode_FreqSynt       (0x02 << 2)
#define OpMode_Transmitter    (0x03 << 2)
#define OpMode_Receiver       (0x04 << 2)

// DataModul //
#define DataModul_DataMode_Packet              (0x00 << 5)
#define DataModul_DataMode_Continous           (0x02 << 5)
#define DataModul_DataMode_ContinousNoBitSync  (0x03 << 5)
#define DataModul_Modulation_Fsk               (0x00 << 3)
#define DataModul_Modulation_Ook               (0x01 << 3)
#define DataModul_ModulationShaping_NoShaping  (0x00 << 0)
#define DataModul_ModulationShaping_BT_1       (0x01 << 0)
#define DataModul_ModulationShaping_BT_05      (0x02 << 0)
#define DataModul_ModulationShaping_BT_03      (0x03 << 0)

//@CMA Calculations//
//*****CMA comments*********
//FRF calculation
//* Frf = Fstep 锟紽rf(23;0)
//
//Frf(23;0)=Frf/Fstep
//****************************
//Fsteps corresponding values:
//57.2204  // 30.0 MHz   CLKOUT
//61.0352  // 32.0 MHz   CLKOUT
//***************************//

//NAM default 915MHz//
#define FrfMsb_915                               0xE4  // Default
#define FrfMid_915                               0xC0  // Default
#define FrfLsb_915                               0x00  // Default

//EMEA default 868MHz//
#define FrfMsb_868                               0xD8  // Default
#define FrfMid_868                               0xFF  // Default
#define FrfLsb_868                               0xF5  // Default

//India default 865MHz//
#define FrfMsb_865                               0xD8  // Default
#define FrfMid_865                               0x3F  // Default
#define FrfLsb_865                               0xF5  // Default

//China default 470MHz//
#define FrfMsb_470                               0x75  // Default
#define FrfMid_470                               0x7F  // Default
#define FrfLsb_470                               0xFA  // Default

//India default 434MHz//
#define FrfMsb_434                               0x6C  // Default
#define FrfMid_434                               0x7F  // Default
#define FrfLsb_434                               0xFA  // Default


#define BitrateMsb_4800   0x1A
#define BitrateLsb_4800   0x0B

#define BitrateMsb_38400  0x03
#define BitrateLsb_38400  0x41

#define BitrateMsb_10000  0x0C
#define BitrateLsb_10000  0x80

#define BitrateMsb_20000  0x06
#define BitrateLsb_20000  0x40

#define BitrateMsb_40000  0x03
#define BitrateLsb_40000  0x20

#define BitrateMsb_50000  0x02
#define BitrateLsb_50000  0x80

#define BitrateMsb_100000 0x01
#define BitrateLsb_100000 0x40

#define BitrateMsb_150000 0x00
#define BitrateLsb_150000 0xD5

#define BitrateMsb_200000 0x00
#define BitrateLsb_200000 0xA0

#define Bitrate_4800      0x1A0B
#define Bitrate_38400     0x0341
#define Bitrate_50000     0x0280
#define Bitrate_100000    0x0140
#define Bitrate_150000    0x00D5
#define Bitrate_200000    0x00A0


// Fdev //
// 4.8 Kbps, modulation index 1
#define FdevMsb_2400   0x00
#define FdevLsb_2400   0x27
// 10 Kbps, modulation index 0.5
#define FdevMsb_2500   0x00
#define FdevLsb_2500   0x29
// 20 Kbps, modulation index 0.5
#define FdevMsb_5000   0x00
#define FdevLsb_5000   0x52
// 40 Kbps, modulation index 0.5
#define FdevMsb_10000  0x00
#define FdevLsb_10000  0xA4
// 50 Kbps, modulation index 1
#define FdevMsb_25000  0x01
#define FdevLsb_25000  0x9A
// 150 Kbps, modulation index 0.5
#define FdevMsb_37500  0x02
#define FdevLsb_37500  0x68
// 100 Kbps, modulation index 1
#define FdevMsb_50000  0x03
#define FdevLsb_50000  0x33
// 200 Kbps, modulation index 0.5 !!!!!
#define FdevMsb_50049  0x03
#define FdevLsb_50049  0x34
// 200 Kbps, modulation index 1
#define FdevMsb_100000 0x06
#define FdevLsb_100000 0x66

#define FdevMsb_19000  0x01
#define FdevLsb_19000  0x37

#define Fdev_5000      0x0052
#define Fdev_19000     0x0137
#define Fdev_25000     0x019A
#define Fdev_50000     0x0333
#define Fdev_50049     0x0334
#define Fdev_100000    0x0666
#define Fdev_170000    0x0AE1
#define Fdev_180000    0x0B85



// Frf //

// 450-470 MHz
// Channel spacing               12.5 KHz
// GL = GH = Channel Spacing / 2 6.25 KHz
// Total number of channels      1599

// Channel bandwidth control //

#define DccFreq_0   (0x00 << 5)
#define DccFreq_1   (0x01 << 5)
#define DccFreq_2   (0x02 << 5)
#define DccFreq_3   (0x03 << 5)
#define DccFreq_4   (0x04 << 5)
#define DccFreq_5   (0x05 << 5)
#define DccFreq_6   (0x06 << 5)
#define DccFreq_7   (0x07 << 5)

#define RxBwMant_0  (0x00 << 3)
#define RxBwMant_1  (0x01 << 3)
#define RxBwMant_2  (0x02 << 3)

#define RxBwExp_0   (0x00 << 0)
#define RxBwExp_1   (0x01 << 0)
#define RxBwExp_2   (0x02 << 0)
#define RxBwExp_3   (0x03 << 0)
#define RxBwExp_4   (0x04 << 0)
#define RxBwExp_5   (0x05 << 0)
#define RxBwExp_6   (0x06 << 0)
#define RxBwExp_7   (0x07 << 0)

#define RxBw_2600   ( (RxBwExp_7) | (RxBwMant_2) )
#define RxBw_3100   ( (RxBwExp_7) | (RxBwMant_1) )
#define RxBw_3900   ( (RxBwExp_7) | (RxBwMant_0) )

#define RxBw_5200   ( (RxBwExp_6) | (RxBwMant_2) )
#define RxBw_6300   ( (RxBwExp_6) | (RxBwMant_1) )
#define RxBw_7800   ( (RxBwExp_6) | (RxBwMant_0) )

#define RxBw_10400  ( (RxBwExp_5) | (RxBwMant_2) )
#define RxBw_12500  ( (RxBwExp_5) | (RxBwMant_1) )
#define RxBw_15600  ( (RxBwExp_5) | (RxBwMant_0) )

#define RxBw_20800  ( (RxBwExp_4) | (RxBwMant_2) )
#define RxBw_25000  ( (RxBwExp_4) | (RxBwMant_1) )
#define RxBw_31300  ( (RxBwExp_4) | (RxBwMant_0) )

#define RxBw_41700  ( (RxBwExp_3) | (RxBwMant_2) )
#define RxBw_50000  ( (RxBwExp_3) | (RxBwMant_1) )
#define RxBw_62500  ( (RxBwExp_3) | (RxBwMant_0) )

#define RxBw_83300  ( (RxBwExp_2) | (RxBwMant_2) )
#define RxBw_100000 ( (RxBwExp_2) | (RxBwMant_1) )
#define RxBw_125000 ( (RxBwExp_2) | (RxBwMant_0) )

#define RxBw_166700 ( (RxBwExp_1) | (RxBwMant_2) )
#define RxBw_200000 ( (RxBwExp_1) | (RxBwMant_1) )
#define RxBw_250000 ( (RxBwExp_1) | (RxBwMant_0) )

#define RxBw_333300 ( (RxBwExp_0) | (RxBwMant_2) )
#define RxBw_400000 ( (RxBwExp_0) | (RxBwMant_1) )
#define RxBw_500000 ( (RxBwExp_0) | (RxBwMant_0) )


// AfcCtrl //

#define AfcCtrl_AfcLowBeta_On   (0x01 << 5)
#define AfcCtrl_AfcLowBeta_Off  (0x00)

// PaLevel //

#define PaLevel_Pa0_On      (0x01 << 7)
#define PaLevel_Pa0_Off     (0x00)

#define PaLevel_Pa1_On      (0x01 << 6)
#define PaLevel_Pa1_Off     (0x00)

#define PaLevel_Pa2_On      (0x01 << 5)
#define PaLevel_Pa2_Off     (0x00)

#define PaLevel_Pa1_Pa2_On      (0x60)
#define PaLevel_Pa1_Pa2_Off     (0x00)

#define PA0_On              (0x01 << 0)
#define PA1_On              (0x01 << 1)
#define PA2_On              (0x01 << 2)


// PaRamp //

#define PaRamp_3400 (0x00)
#define PaRamp_2000 (0x01)
#define PaRamp_1000 (0x02)
#define PaRamp_500  (0x03)
#define PaRamp_250  (0x04)
#define PaRamp_125  (0x05)
#define PaRamp_100  (0x06)
#define PaRamp_62   (0x07)
#define PaRamp_50   (0x08)
#define PaRamp_40   (0x09)
#define PaRamp_31   (0x0A)
#define PaRamp_25   (0x0B)
#define PaRamp_20   (0x0C)
#define PaRamp_15   (0x0D)
#define PaRamp_12   (0x0E)
#define PaRamp_10   (0x0F)

// RegOcp //

#define Ocp_Ocp_On  (0x01 << 4)
#define Ocp_Ocp_Off (0x00)

// RegLna //

#define Lna_LnaZin_200  (0x01 << 7)
#define Lna_LnaZin_50 (0x00)

#define Lna_LnaGain_Agc          (0x00)
#define Lna_LnaGain_MaxGain      (0x01)
#define Lna_LnaGain_MaxGain_6    (0x02)
#define Lna_LnaGain_MaxGain_12   (0x04)
#define Lna_LnaGain_MaxGain_24   (0x05)
#define Lna_LnaGain_MaxGain_36   (0x06)
#define Lna_LnaGain_MaxGain_48   (0x07)

// RegAfcFei //

#define AfcFei_FeiDone           (0x01 << 6) //read only
#define AfcFei_FeiStart          (0x01 << 5)

#define AfcFei_AfcDone           (0x01 << 4) //read only
#define AfcFei_AfcAutoClear_Off  (0x00)
#define AfcFei_AfcAutoClear_On   (0x01 << 3)
#define AfcFei_AfcAuto_Off       (0x00)
#define AfcFei_AfcAuto_On        (0x01 << 2)
#define AfcFei_AfcClear          (0x01 << 1)
#define AfcFei_AfcStart          (0x01 << 0)


// RegRssi //

#define Rssi_RssiDone  (0x01 << 1) //read only
#define Rssi_RssiStart (0x01 << 0)

// RegSyncConfig //

#define SyncConfig_Sync_On                    (0x01 << 7)
#define SyncConfig_Sync_Off                   (0x00)

#define SyncConfig_FifioFill_ifSyncAddres     (0x00)
#define SyncConfig_FifioFill_ifFifoFillisSet  (0x01 << 6)

#define SyncConfig_SyncSize_1                 (0x00 << 3)
#define SyncConfig_SyncSize_2                 (0x01 << 3)
#define SyncConfig_SyncSize_3                 (0x02 << 3)
#define SyncConfig_SyncSize_4                 (0x03 << 3)
#define SyncConfig_SyncSize_5                 (0x04 << 3)
#define SyncConfig_SyncSize_6                 (0x05 << 3)
#define SyncConfig_SyncSize_7                 (0x06 << 3)
#define SyncConfig_SyncSize_8                 (0x07 << 3)

// RegPacketConfig1 //

#define PacketConfig1_PacketFormat_Fixed      (0x00)
#define PacketConfig1_PacketFormat_Variable   (0x01 << 7)

#define PacketConfig1_DcFree_None             (0x00 << 5)
#define PacketConfig1_DcFree_Manchester       (0x01 << 5)
#define PacketConfig1_DcFree_Whitening        (0x02 << 5)

#define PacketConfig1_Crc_On                  (0x01 << 4)
#define PacketConfig1_Crc_Off                 (0x00 << 4)

#define PacketConfig1_CrcAutoClear_On         (0x00 << 3)
#define PacketConfig1_CrcAutoClear_Off        (0x01 << 3)

#define PacketConfig1_AddresFiltering_Off     (0x00)

// RegPacketConfig2 //

#define PacketConfig2_Aes_Off           (0x00)
#define PacketConfig2_Aes_On            (0x01 << 0)

#define PacketConfig2_AutoRxRestart_Off (0x00 << 1)
#define PacketConfig2_AutoRxRestart_On  (0x01 << 1)

#define PacketConfig2_RxRestart         (0x01 << 2)

// Reg_IrqFlags1 //
#define IrqFlags1_ModeReady             (0x01 << 7)

#endif // _RF_H //
//=============================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，
//     不足之处，欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学飞思卡尔嵌入式中心（苏州华祥信息科技有限公司）
//技术咨询：0512-65214835  http://sumcu.suda.edu.cn
//业务咨询：0512-87661670,18915522016  http://www.hxtek.com.cn
