//=========================================================================
//文件名称：rf.c
//=========================================================================
#include "rf.h"

#define DEFAULT_FRF_434       //通信频率


//硬件地址变量


//无线模块寄存器映射表
uint_8 static const MKW01DefaultRegisterValues[] =
{
    // 无线收发器操作模式设置@0x01//
    MKW01_Reg_OpMode, (uint_8) ( OpMode_Sequencer_On | OpMode_Listen_Off
    		                                         | OpMode_StandBy ) ,
    
    // 无线数据模式和调制（modulation）设置 @0x02//
    MKW01_Reg_DataModul, (uint_8) ( DataModul_DataMode_Packet
          | DataModul_Modulation_Fsk | DataModul_ModulationShaping_BT_05) ,
    
    // 无线比特率设置@0x03-0x04//
    MKW01_Reg_BitrateMsb, (uint_8) ( BitrateMsb_100000 ) ,
    MKW01_Reg_BitrateLsb, (uint_8) ( BitrateLsb_100000 ) ,
    
    // 无线频率偏差（偏移）（frequency deviation）设置@0x05-0x06//
    MKW01_Reg_FdevMsb, (uint_8) ( FdevMsb_2400 ) ,
    MKW01_Reg_FdevLsb, (uint_8) ( FdevLsb_2400 ) ,
    
    //设置所用频段
    MKW01_Reg_FrfMsb, (uint_8) ( 0x6C ),
    MKW01_Reg_FrfMid, (uint_8) ( 0x40 ),
    MKW01_Reg_FrfLsb, (uint_8) ( 0x00 ),
    
    // 无线在低的调制指数情况下是否采用 AFC（自动频率校正）控制 @0x0B//
    MKW01_Reg_AfcCtrl, (uint_8) AfcCtrl_AfcLowBeta_Off ,
    
    // 无线PA选择和输出功率初始化 （output power） @0x11//
    //MKW01_Reg_PaLevel, (uint_8) ( PaLevel_Pa0_On | PaLevel_Pa1_Off
    //		                                   | PaLevel_Pa2_Off | 0x10 ) ,
    //0-31(功率)，不影响
    MKW01_Reg_PaLevel, (uint_8) ( PaLevel_Pa0_On | PaLevel_Pa1_Off
    		                                   | PaLevel_Pa2_Off | 31 ) ,


	// 在FSK模式中控制PA的ramp时间设置@0x12//
    MKW01_Reg_PaRamp, (uint_8) (PaRamp_40) ,
    
    // 无线PA过流保护设置 0x13//
    MKW01_Reg_Ocp, (uint_8) ( Ocp_Ocp_On | 0x0C) ,
    
    // 无线低噪音放大器（LNA）增益和输入阻抗（impedance）初始化@0x18//
//    MKW01_Reg_Lna, (uint_8) ( Lna_LnaZin_50 | Lna_LnaGain_Agc ) ,
    //2015-1-16 GZF注释
    //此处设置表示接收时，实际增益=（最大增益-12）；当接收距离过远时，
    //推荐使用自动增益控制：Lna_LnaGain_Agc，
    //同时请参考KW01参考手册第66页LNA相关内容进行设置(GZF 2015-1-16)
    MKW01_Reg_Lna, (uint_8) ( Lna_LnaZin_50 | Lna_LnaGain_MaxGain_12 ) ,
    
    // 无线通道滤波带宽设置@0x19//
    MKW01_Reg_RxBw, ( DccFreq_7 | RxBw_10400 ) ,
    
    // AFC操作模式下通道滤波器设置@0x1A//
    MKW01_Reg_AfcBw, ( DccFreq_7 | RxBw_10400 ) ,
    
    // 无线自动频率控制设置@0x1E//
    MKW01_Reg_AfcFei, (uint_8) ( AfcFei_AfcAuto_Off
    		                                | AfcFei_AfcAutoClear_On ) ,
    
    // 无线 RSSI（接收信号强度）阈值设置@0x29//
    // RSSIthres = [-174 + NF +10*log(2*RxBw) + DemodSNR] dBm
    // NF = 7dB
    //DemodSnr = 8dB //RxBw depends on frequency bands and profiles
    // -101 dBm for 333.3 Khz singleside channel filter bandwith
    MKW01_Reg_RssiThresh, (uint_8) (0xDC) ,
    
    // 无线RX接收请求   与   探测到RSSI之间的  超时时间  设置 @0x2A//
    MKW01_Reg_RxTimeout1, (uint_8) 0x00 , //disable timeout rx start
    // 无线探测到RSSI与加载完毕（RX PayloadReady） 之间的 超时时间设置@0x2B//
    MKW01_Reg_RxTimeout2, (uint_8) 0x00 , //disable timeout rx start
    
    // MKW01报头 （preamble）大小设置 @0x2C-0x2D//
    MKW01_Reg_PreambleMsb, (uint_8) 0x00,
    MKW01_Reg_PreambleLsb, (uint_8) 0x10,
    
    // 无线同步字识别控制和值设置@0x2E-0x30//
    MKW01_Reg_SyncConfig, (uint_8) (SyncConfig_Sync_On
    		| SyncConfig_FifioFill_ifSyncAddres | SyncConfig_SyncSize_2) ,
    //SFD value for uncoded with phySUNMRFSKSFD = 0
    MKW01_Reg_SyncValue1, (uint_8) (0x90) ,
    //SFD value for uncoded with phySUNMRFSKSFD = 0
    MKW01_Reg_SyncValue2, (uint_8) (0x4E) ,
    
    // 无线包模式设置@0x37//
    //1 变长包格式； 2DC-free数据白化； 3CRC校验开启 ；4  当CRC校验错误时不自动
    //清除FIFO中的数据，产生PayloadReady中断；5 禁止地址过滤
    //    MKW01_Reg_PacketConfig1, 0xD8,
    //1 变长包格式； 2DC-free数据白化； 3CRC校验开启 ；4  当CRC校验错误时自动
    //清除FIFO中的数据，不产生PayloadReady中断；5 禁止地址过滤
    //    MKW01_Reg_PacketConfig1, 0xD0,
    //1 变长包格式； 2DC-free数据白化； 3CRC校验开启 ；4  当CRC校验错误时自动
    //清除FIFO中的数据，不产生PayloadReady中断；5启用地址过滤 （必须匹配HW_ADR）
    //MKW01_Reg_PacketConfig1, 0xD2,
    //1 定长包格式； 2DC-free数据白化； 3CRC校验开启 ；4  当CRC校验错误时自动
    //清除FIFO中的数据，不产生PayloadReady中断；5启用地址过滤 （必须匹配HW_ADR）
    MKW01_Reg_PacketConfig1, 0x52,
    
    MKW01_Reg_NodeAdrs, 0x53,    //硬件过滤地址
    
    // 无线包模式设置@0x3D  在连续模式强制接收器等待模式可用，进制AES加解密//
    MKW01_Reg_PacketConfig2, 0x00,
//    // 无线包模式设置@0x3D  匹配PARamp时间，开启自动重启RX模式//
//    MKW01_Reg_PacketConfig2, 0x32,
    
    //无线负载（payload）长度设置@0x38//
    //之前设置PacketFormat=1，长度可变，这个寄存器表示RX的最大长度，TX中不算。
    MKW01_Reg_PayloadLength, (uint_8) PAY_LOAD_LENGTH  //即rx的最大长度
};

static int MACexp2(uint_8 zhishu);
//static void Old_RF_SendData(uint_8 dataLength,uint_8* data);
static void MKW01Drv_ConfigureDIOPinsFast(uint_8 regDio1, uint_8 regDio2);

static void MKW01Drv_SetOperatingMode(uint_8 mode);




//对外接口函数

//=========================================================================
//函数名称：RF_Init
//功能概要：向无线模块寄存器写入初值
//参数说明：无
//函数返回：无
//=========================================================================
void RF_Init(uint_8 HW_ADR)
{
    uint_8 idx;
    //HW_ADR = 0x53;
    for(idx = 0; idx < (sizeof(MKW01DefaultRegisterValues)
    		             /sizeof(MKW01DefaultRegisterValues[0])); idx += 2)
    {
        MKW01Drv_WriteRegister(MKW01DefaultRegisterValues[idx],
        		                        MKW01DefaultRegisterValues[idx+1]);
    }

    MKW01Drv_WriteRegister(MKW01DefaultRegisterValues[52], HW_ADR);

    //===================================================================
   //Fading Margin Improvement 0x10:Improved margin,use if AfcLowBetaOn = 1
    MKW01Drv_WriteRegister(MKW01_Reg_TestDagc, 0x10);
    //设置比特率
    MKW01Drv_WriteRegister(MKW01_Reg_BitrateMsb,
    		                           (uint_8) ( Bitrate_150000 >> 8  ) );
    MKW01Drv_WriteRegister(MKW01_Reg_BitrateLsb,
    		                         (uint_8) ( Bitrate_150000 & 0x00FF) );
    //设置频率偏差
    MKW01Drv_WriteRegister(MKW01_Reg_FdevMsb, (uint_8) (Fdev_170000 >> 8));
    MKW01Drv_WriteRegister(MKW01_Reg_FdevLsb,
    		                           (uint_8) ( Fdev_170000 & 0x00FF ) );

    MKW01Drv_WriteRegister(MKW01_Reg_RxBw, (DccFreq_2 | RxBw_333300));
    MKW01Drv_WriteRegister(MKW01_Reg_AfcBw, (DccFreq_2 | RxBw_333300));

    MKW01Drv_ConfigureDIOPinsFast((DIO0_RxPayloadReady | DIO1_RxFifoLevel),
    		                             (DIO4_RxTimeout | DIO5_RxClkOut));
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; // 清标志为
    PORTC_PCR4 |= PORT_PCR_IRQC(9);  // 设置上升沿中断
    NVIC_ICPR |= 0x80000000;         // 重新使能D口中断
    NVIC_ISER |= 0x80000000;
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
}


//=========================================================================
//函数名称：RF_SendData
//功能概要：RF模块发送函数
//参数说明：dataLength:有效数据长度；data:数据区缓冲区。
//函数返回：无
//=========================================================================
void RF_SendData(uint_8 dataLength,uint_8* data,uint_8 HW_ADR)
{
    uint_8 blockIndex;
    uint_8 x1;
    uint_8 checkNum = 0;
    //越界返回（减2是因为其中包含长度字节与地址字节）
    if(dataLength >= (PAY_LOAD_LENGTH - 2) )
        return;

    //1.准备发送
    NVIC_ICER = 0x80000000;//禁止 DIO0,DIO1 and DIO4 中断
    //将收发器强制切换到StandBy状态
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    //当FIFONotEmpty非空时，清FIFO
    while(MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40)
        (void)MKW01Drv_ReadRegister(MKW01_Reg_Fifo);
    //2.设置FIFO阈值和发送开始条件，FIFO中的数据字节数超过fifoThreshold才发送包
    MKW01Drv_WriteRegister(MKW01_Reg_FifoThresh,
    		                        ((uint_8)(PAY_LOAD_LENGTH -1) & 0x7F));

    x1 = IntDisableAll();

    //3.填充FIFO（2字节）
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, HW_ADR);       //填充硬件过滤地址
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, dataLength);   //填充有效长度字节
    checkNum = HW_ADR + dataLength; //计算校验和
    for(blockIndex = 0;blockIndex < dataLength; blockIndex++)
    {
    	//填充有效数据
        WAIT_SPITX_BUFFER_EMPTY;
        MKW01Drv_WriteRegister(MKW01_Reg_Fifo, data[blockIndex]);

        checkNum += data[blockIndex];//计算校验和
    }
    for( ;blockIndex < PAY_LOAD_LENGTH-4 ; blockIndex++)
    {
    	//填充无效数据（用零填充）
        WAIT_SPITX_BUFFER_EMPTY;
        MKW01Drv_WriteRegister(MKW01_Reg_Fifo, 0);
    }

    //4.添加一字节校验和与保留一个数据（2字节））
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, checkNum);
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, 0);

    IntRestoreAll(x1) ;
    //5.配置引脚DIO复用
    //MKW01Drv_ConfigureDIOPins((DIO0_TxPacketSent | DIO1_TxFifoLevel
    //  | DIO2_TxLowBat | DIO3_TxLowBat), (DIO4_TxLowBat | DIO5_TxClkOut));

    //TX should be enabled until this line, and the Interrupt Flag set.
    MKW01Drv_WriteRegister(MKW01_Reg_OpMode, (uint_8) ( OpMode_Sequencer_On
    		                    | OpMode_Listen_Off | OpMode_Transmitter));
    Delay_ms(5);  //经试验测试2015-8-13
    //当FIFONotEmpty非空时，等待
    while(MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40);

    // Write logic 1 for cleaning the interrupt flag.
    PORTC_PCR4 |= PORT_PCR_ISF_MASK;
    //Enable the interrupt when with the desired sensitivity
    PORTC_PCR4 |= PORT_PCR_IRQC(0x0A);

    NVIC_ICPR |= 0x80000000;
    NVIC_ISER |= 0x80000000;
    MKW01Drv_SetOperatingMode(OpMode_Receiver);

}

//=========================================================================
//函数名称：RFSendDataByCSMACA
//功能概要：RF采用CSMA/CA算法发送一帧
//参数说明：DataAdr--要发送数据的首地址;DataLen--要发送数据字节数
//函数返回：无
//内部调用：能量检测RFEnergyDetect
//附加说明：本程序中采用CSMA/CA算法，即首先延时一个随机(2^BE范围内 BE=minBE)的时间，
//         然后判断信道是否空闲，如果空闲发送，如果不空闲随机延时更长的时间(BE++,
//         在BE和maxBE之间延时)。可以循环(NB<maxNB)次,维持两个变量BE和NB。
//         BE初始化为minBE。每次延时都是在(2^BE-1)的范围内随机延时一个时间。若信道
//         不空闲BE=min(BE+1,maxBE)。NB为循环变量
//         maxNB--CSMA/CA算法中：表示最多尝试发送的次数，如果最后还是发送失败了，
//         那么会强制发送一次
//         maxBE--CSMA/CA算法中：表示退避指数的最大值
//=========================================================================
void RFSendDataByCSMACA(uint_8 DataLen, uint_8 *DataAdr,uint_8 isEnergyDetect,uint_8 HW_ADR)
{
	uint_8 maxNB,maxBE;
	uint_8 NB,BE;
	uint_32 i,j;
	//
	maxNB=50;     //
	maxBE=6;      //
	NB = 0;       //NB表示当前尝试发送的次数
	BE = 1;       //BE代表指数随机退避的指数值，随机退避的时间选择在2^BE和0之间

    for(NB =0; NB < maxNB; NB++)
    {
        //检测信道是否空闲：如果空闲发送数据
        i=0x85;    //不检测能量，用一个大值
        //如果需要测得能量值，isEnergyDetect值为1
    	if(1==isEnergyDetect)   i = RFEnergyDetect();
        //如果能量值大于AD，那么进行发送  手册推荐值为0x52对应的是-82dBM
        if(i >= ENERGY_SEND_MIN) break;
    	//在(0-2^BE)范围内随机等待一个时间
        //如果信道不空闲：设置BE增加，如果BE比设定的最大值还大，那么设置为最大值
        BE = (BE < maxBE) ? (BE + 1) : maxBE;
        j = rand() % MACexp2(BE)+1;
        Delay_ms(j);
        //重新设置一下随机数的种子，也就是该种子依赖与上次的随机值(暂时不实现)
    }
    //发送
    RF_SendData(DataLen,DataAdr,HW_ADR);
}

//------------------内部调用函数声明--------------------------------------
//=========================================================================
//函数名称：MKW01Drv_ConfigureDIOPinsFast
//功能概要：配置DIO工作模式
//参数说明：regDio1:DIOMapping1寄存器值
//         regDio2：DIOMapping2寄存器值
//函数返回：无
//=========================================================================
static void MKW01Drv_ConfigureDIOPinsFast(uint_8 regDio1, uint_8 regDio2)
{
    uint_8 reg2;
    MKW01Drv_WriteRegisterFastMacro(MKW01_Reg_DioMapping1, regDio1);
    MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_DioMapping2, reg2);
    reg2 = (uint_8) (reg2 & 0x07);
    reg2 = (uint_8) (reg2 | (regDio2 & 0xF0) );
    MKW01Drv_WriteRegisterFastMacro(MKW01_Reg_DioMapping2, reg2);
}

//=========================================================================
//函数名称：MKW01Drv_SetOperatingMode
//功能概要：设置RF工作模式
//参数说明：mode:RF工作模式
//函数返回：无
//=========================================================================
static void MKW01Drv_SetOperatingMode(uint_8 mode)
{
    uint_8 regOpMode;
    regOpMode = MKW01Drv_ReadRegister(MKW01_Reg_OpMode);
    regOpMode = (regOpMode & 0xE3) | mode;
    MKW01Drv_WriteRegister(MKW01_Reg_OpMode, regOpMode);
    while ( ( MKW01Drv_ReadRegister( MKW01_Reg_IrqFlags1)
    		                            & IrqFlags1_ModeReady) == 0x00 );
}
//------------------内部调用函数声明结束--------------------------------------



//=========================================================================
//函数名称：MKW01Drv_IRQ_DIO1_Enable
//功能概要：使能IRQ1
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_IRQ_DIO1_Enable(void)
{
	// TODO! Need to enable Sensitivity parameter
    PORTC_PCR4 |= PORT_PCR_ISF_MASK;   // 清中断标志位
    PORTC_PCR4 |= PORT_PCR_IRQC(9);    // 设置中断类型
}



//没用



//=========================================================================
//函数名称：RFEnergyDetect
//功能概要：读取RSSI值
//参数说明：注意，返回的RSSI值为其绝对值，用户在使用时应取负数，如函数返回5，
//         其实际RSSI值为-5
//函数返回：无
//=========================================================================
uint_8 RFEnergyDetect(void)
{
    uint_8 Rssi = 0;
    //将收发器置为待机模式，之后再从FIFO中读出接收到的数据（保证不被中断走）
    MKW01Drv_SetOperatingMode(OpMode_StandBy);

    //等待状态转换完成，【GZF 2015-1-16 添加 】
//    while ((MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags1)
//    		& IrqFlags1_ModeReady) == 0x00);
    Rssi = MKW01Drv_ReadRegister(MKW01_Reg_RssiValue) / 2;
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
    //等待状态转换完成,【GZF 2015-1-16 添加 】
//    while ((MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags1)
//    		& IrqFlags1_ModeReady) == 0x00);
    return Rssi;
}
//=========================================================================
//函数名称：RFTempDetect
//功能概要：读取rf温度值
//参数说明：无
//函数返回：无
//=========================================================================
uint_8 RFTempDetect(void)
{
    uint_8 tmpI=0;
    //1)将MKW01_Reg_Temp1的第3位设置为1，启动温度测量
    MKW01Drv_SetOperatingMode(OpMode_StandBy);

    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    tmpI=tmpI|(1<<3);
    MKW01Drv_WriteRegister(MKW01_Reg_Temp1,tmpI);
    //2)判读MKW01_Reg_Temp1的第2位，=1:正在测量温度，
    //  =0:测量完成,温度值存放在MKW01_Reg_Temp2
    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    //uart_send1(UART_0,tmpI);
    while((tmpI&(1<<2))!=0)
    {
    	tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    	//uart_send1(UART_0,tmpI);
    }//等待测量完成

    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp2);
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
    return tmpI;
}

//=========================================================================
//函数名称：RF_ReceiveFrame
//功能概要：RF模块接收一帧
//参数说明：pbuf：接收数据首地址;plen:接收数据长度有效指针
//         （接收数据长度ReDataLen<=60字节）
//         附加带出一个字节：能量值=pbuf[ReDataLen]
//函数返回：接收状态标志位（=0,接收正常，=其他值，接收异常）
//备          注： 20141126 WYH 改
//=========================================================================
uint_8 RF_ReceiveFrame(uint_8* pbuf, uint_8* plen,uint_8 HW_ADR )
{
	uint_8 flag;
    uint_8 Rssi;
    uint_8 checkadr;
    uint_8 checkNum ;
    uint_8 ReDataLen;
    uint_32 i;

    //1. 临时变量初始化
    flag = 0;
    Rssi = 0;
    checkadr=0;
    checkNum = 0;
    ReDataLen=0;

    Delay_ms(18);    //经过试验测试2015-8-13

    //2. 将收发器置为待机模式，之后再从FIFO中读出接收到的数据（保证不被中断走）
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    Rssi = MKW01Drv_ReadRegister(MKW01_Reg_RssiValue) / 2;  //能量值

    //3. 读出硬件过滤地址（1字节）,并进行地址校验
    //读出硬件过滤地址
    MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, checkadr);
    checkNum += checkadr;                            //计算校验和
    if(checkadr != HW_ADR) goto RF_ReceiveFrame_err; //硬件地址校验错误

    //4. 硬件地址校验正确，继续读下一字节（即为接收数据帧有效长度）
    //读出接收数据帧有效长度字节
    MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, ReDataLen);
    checkNum += ReDataLen;                           //计算校验和

    //5. 读出数据（PAY_LOAD_LENGTH-4个字节）
    i=0;
    //FIFO非空则将其中的数据读出
    while( (MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40) )
    {
    	//将数据读到pbuf[i]中
        MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, pbuf[i]);
        if(i < (PAY_LOAD_LENGTH - 4)) checkNum += pbuf[i];//继续计算校验和
        i++;
    }
    //6. pbuf[PAY_LOAD_LENGTH - 4]字节是接收来的校验和（累加校验）
    //7. 判断接收个数以及校验和是否正确
    if (!(i == (PAY_LOAD_LENGTH - 2)
    		                     && (checkNum == pbuf[PAY_LOAD_LENGTH-4])))
    	goto RF_ReceiveFrame_err;           //接收个数或校验和错误
    //8. 接收正确
    	 flag = 0;          //冗余赋值
    	 *plen = ReDataLen;   //带回接收帧的长度

//        //20141124 添加 判断重帧
//		for(j=0;j<(PAY_LOAD_LENGTH - 4);j++)
//		{
//			if (rf_pre_redata[j] != pbuf[j]) break;
//		}
//		if(j==PAY_LOAD_LENGTH - 4) goto RF_ReceiveFrame_err;   //
//	   	for(j=0;j<(PAY_LOAD_LENGTH - 4);j++) rf_pre_redata[j] = pbuf[j];
	   	//在pbuf[ReDataLen]中带出接收能量值
	   	pbuf[ReDataLen]=Rssi;
	   	goto RF_ReceiveFrame_exit;

		RF_ReceiveFrame_err:
				flag = 1;            //接收失败标志位
		RF_ReceiveFrame_exit:
		//清FIFO
		while(MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40)
			   (void)MKW01Drv_ReadRegister(MKW01_Reg_Fifo);
		//重新设置为接收模式
		MKW01Drv_SetOperatingMode(OpMode_Receiver);
		return flag;
}

//=========================================================================
//函数名称：MKW01xDrv_IRQ_PortConfig
//功能概要：配置MCU对应的连接在无线模块DIO上的中断
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01xDrv_IRQ_PortConfig(void)
{
    //Enable PORTC and PORTD interrupts
	//Single interrupt vector for Port C and Port D
    NVIC_ICPR |= 0x80000000;
    NVIC_ISER |= 0x80000000;
    //PORT C and PORT D Clock Gate Control enable//
    SIM_SCGC5 |= (1<<12) | (1<<11);
    PORTC_PCR3 |= PORT_PCR_MUX(1);         //DIO0    //PORTC_PCR3. (MUX=1)
    PORTC_PCR4 |= PORT_PCR_MUX(1);         //DIO1    //PORTC_PCR4. (MUX=1)
    PORTD_PCR4 |= PORT_PCR_MUX(1);         //DIO4    //PORTD_PCR4. (MUX=1)
}
//=========================================================================
//函数名称：MKW01Drv_ConfigureDIOPins
//功能概要：配置DIO引脚，以便于MCU了解与控制无线收发器的状态
//参数说明：regDio1：DIO1寄存器；regDio2：DIO2寄存器
//函数返回：无
//=========================================================================
void MKW01Drv_ConfigureDIOPins(uint_8 regDio1, uint_8 regDio2)
{
    uint_8 reg2;
    MKW01Drv_WriteRegister(MKW01_Reg_DioMapping1, regDio1);
    reg2 = MKW01Drv_ReadRegister(MKW01_Reg_DioMapping2);
    reg2 = (uint_8) (reg2 & 0x07);
    reg2 = (uint_8) (reg2 | (regDio2 & 0xF0) );
    MKW01Drv_WriteRegister(MKW01_Reg_DioMapping2, reg2);
}

//=========================================================================
//函数名称：MKW01Drv_RadioStandByReq
//功能概要：将RF置为StandBy模式
//参数说明：无
//函数返回：无
//=========================================================================
uint_8 MKW01Drv_RadioStandByReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    return 0;
}

//=========================================================================
//函数名称：MKW01Drv_RadioSleepReq
//功能概要：将RF置为Sleep模式
//参数说明：无
//函数返回：无
//=========================================================================
uint_8 MKW01Drv_RadioSleepReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_Sleep);
    return 0;
}

//=========================================================================
//函数名称：MKW01Drv_RadioWakeUpReq
//功能概要：将RF置为唤醒模式
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_RadioWakeUpReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
}

//=========================================================================
//函数名称：MKW01Drv_RC_Calibration
//功能概要：RC振荡器校准，如果硬件版本为V2a，上电时应进行RC校准//
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_RC_Calibration(void)                                              
{
    volatile uint_8 dummy;
    dummy = MKW01Drv_ReadRegister(MKW01_Reg_Version);
    if(MKW01Drv_ReadRegister(MKW01_Reg_Version) == 0x21)                          
    {
    	//set transceiver in StandBy mode
        MKW01Drv_WriteRegister(MKW01_Reg_OpMode, 0x04);
        MKW01Drv_WriteRegister(0x57, 0x80);
        MKW01Drv_WriteRegister(MKW01_Reg_Osc1,
        		           (MKW01Drv_ReadRegister(MKW01_Reg_Osc1) | 0x80));
        while((MKW01Drv_ReadRegister(0x0A) & 0x40) == 0x00);
        MKW01Drv_WriteRegister(MKW01_Reg_Osc1,
        		           (MKW01Drv_ReadRegister(MKW01_Reg_Osc1) | 0x80));
        while((MKW01Drv_ReadRegister(MKW01_Reg_Osc1) & 0x40) == 0x00);
        MKW01Drv_WriteRegister(0x57, 0x00);
    }
}

//=========================================================================
//函数名称：MKW01Drv_IRQ_Enable
//功能概要：设置引脚中断类型
//参数说明：SensitivityDIO0：中断类型   SensitivityDIO1：中断类型
//函数返回：无
//=========================================================================
void MKW01Drv_IRQ_Enable(uint_8 SensitivityDIO0, uint_8 SensitivityDIO1)
{

    PORTC_PCR3 |= PORT_PCR_ISF_MASK; // 清中断标志位
    PORTC_PCR3 |= PORT_PCR_IRQC(SensitivityDIO0);  //设置中断类型
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; // 清中断标志位
    PORTC_PCR4 |= PORT_PCR_IRQC(SensitivityDIO1);  //设置中断类型
}

//=========================================================================
//函数名称：MKW01Drv_CCA_Enable
//功能概要：使能CCA
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_CCA_Enable(void)
{

    PORTD_PCR4 |= PORT_PCR_ISF_MASK; // 清中断标志位
    PORTD_PCR4 |= PORT_PCR_IRQC(9);  // 设置中断类型
}

//=========================================================================
//函数名称：MKW01Drv_IRQ_Clear
//功能概要：清IRQ中断
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_IRQ_Clear(  void)
{

    PORTC_PCR3 |= PORT_PCR_ISF_MASK; //清PORTC_PCR3中断
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; //清PORTC_PCR4中断
 
}

//=========================================================================
//函数名称：MKW01Drv_CCA_Clear
//功能概要：清IRQ中断
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_CCA_Clear(void)
{
    PORTD_PCR4 |= PORT_PCR_ISF_MASK; //清PORTD_PCR4中断
}

//=========================================================================
//函数名称：MKW01Drv_IRQ_Disable
//功能概要：禁止IRQ
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_IRQ_Disable(void)
{
    PORTC_PCR3 &= ~PORT_PCR_IRQC_MASK;
    PORTC_PCR4 &= ~PORT_PCR_IRQC_MASK; 
}

//=========================================================================
//函数名称：MKW01Drv_IRQ_DIO1_Disable
//功能概要：禁止DIO1模块
//参数说明：无
//函数返回：无
//=========================================================================
void MKW01Drv_IRQ_DIO1_Disable(void)
{
    PORTC_PCR4 &= ~PORT_PCR_IRQC_MASK; 
}

void MKW01Drv_IRQ_DIO0_Enable(void)
{
	//TODO! Need to enable Sensitivity parameter
	 // Write logic 1 for cleaning the interrupt flag.
    PORTC_PCR3 |= PORT_PCR_ISF_MASK;
    //Enable the interrupt when rising edge occurs
    PORTC_PCR3 |= PORT_PCR_IRQC(9);
}

void MKW01Drv_IRQ_DIO0_Disable(void)
{
    PORTC_PCR3 &= ~PORT_PCR_IRQC_MASK; 
}

void MKW01Drv_CCA_Disable(void)
{
//    MKW01_IRQ_DIO4_PCR &= ~PORT_PCR_IRQC_MASK; 
    PORTD_PCR4 &= ~PORT_PCR_IRQC_MASK; 
}

//给出指数和底数，算出结果
static int MACexp2(uint_8 zhishu)
{
    int i = 0;
    int result = 1;
    for(i = 0; i < zhishu; i++)
    {
        result = result<<1;
    }
    return result;
}




