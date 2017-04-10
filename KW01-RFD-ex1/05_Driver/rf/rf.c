//=========================================================================
//�ļ����ƣ�rf.c
//=========================================================================
#include "rf.h"

#define DEFAULT_FRF_434       //ͨ��Ƶ��


//Ӳ����ַ����


//����ģ��Ĵ���ӳ���
uint_8 static const MKW01DefaultRegisterValues[] =
{
    // �����շ�������ģʽ����@0x01//
    MKW01_Reg_OpMode, (uint_8) ( OpMode_Sequencer_On | OpMode_Listen_Off
    		                                         | OpMode_StandBy ) ,
    
    // ��������ģʽ�͵��ƣ�modulation������ @0x02//
    MKW01_Reg_DataModul, (uint_8) ( DataModul_DataMode_Packet
          | DataModul_Modulation_Fsk | DataModul_ModulationShaping_BT_05) ,
    
    // ���߱���������@0x03-0x04//
    MKW01_Reg_BitrateMsb, (uint_8) ( BitrateMsb_100000 ) ,
    MKW01_Reg_BitrateLsb, (uint_8) ( BitrateLsb_100000 ) ,
    
    // ����Ƶ��ƫ�ƫ�ƣ���frequency deviation������@0x05-0x06//
    MKW01_Reg_FdevMsb, (uint_8) ( FdevMsb_2400 ) ,
    MKW01_Reg_FdevLsb, (uint_8) ( FdevLsb_2400 ) ,
    
    //��������Ƶ��
    MKW01_Reg_FrfMsb, (uint_8) ( 0x6C ),
    MKW01_Reg_FrfMid, (uint_8) ( 0x40 ),
    MKW01_Reg_FrfLsb, (uint_8) ( 0x00 ),
    
    // �����ڵ͵ĵ���ָ��������Ƿ���� AFC���Զ�Ƶ��У�������� @0x0B//
    MKW01_Reg_AfcCtrl, (uint_8) AfcCtrl_AfcLowBeta_Off ,
    
    // ����PAѡ���������ʳ�ʼ�� ��output power�� @0x11//
    //MKW01_Reg_PaLevel, (uint_8) ( PaLevel_Pa0_On | PaLevel_Pa1_Off
    //		                                   | PaLevel_Pa2_Off | 0x10 ) ,
    //0-31(����)����Ӱ��
    MKW01_Reg_PaLevel, (uint_8) ( PaLevel_Pa0_On | PaLevel_Pa1_Off
    		                                   | PaLevel_Pa2_Off | 31 ) ,


	// ��FSKģʽ�п���PA��rampʱ������@0x12//
    MKW01_Reg_PaRamp, (uint_8) (PaRamp_40) ,
    
    // ����PA������������ 0x13//
    MKW01_Reg_Ocp, (uint_8) ( Ocp_Ocp_On | 0x0C) ,
    
    // ���ߵ������Ŵ�����LNA������������迹��impedance����ʼ��@0x18//
//    MKW01_Reg_Lna, (uint_8) ( Lna_LnaZin_50 | Lna_LnaGain_Agc ) ,
    //2015-1-16 GZFע��
    //�˴����ñ�ʾ����ʱ��ʵ������=���������-12���������վ����Զʱ��
    //�Ƽ�ʹ���Զ�������ƣ�Lna_LnaGain_Agc��
    //ͬʱ��ο�KW01�ο��ֲ��66ҳLNA������ݽ�������(GZF 2015-1-16)
    MKW01_Reg_Lna, (uint_8) ( Lna_LnaZin_50 | Lna_LnaGain_MaxGain_12 ) ,
    
    // ����ͨ���˲���������@0x19//
    MKW01_Reg_RxBw, ( DccFreq_7 | RxBw_10400 ) ,
    
    // AFC����ģʽ��ͨ���˲�������@0x1A//
    MKW01_Reg_AfcBw, ( DccFreq_7 | RxBw_10400 ) ,
    
    // �����Զ�Ƶ�ʿ�������@0x1E//
    MKW01_Reg_AfcFei, (uint_8) ( AfcFei_AfcAuto_Off
    		                                | AfcFei_AfcAutoClear_On ) ,
    
    // ���� RSSI�������ź�ǿ�ȣ���ֵ����@0x29//
    // RSSIthres = [-174 + NF +10*log(2*RxBw) + DemodSNR] dBm
    // NF = 7dB
    //DemodSnr = 8dB //RxBw depends on frequency bands and profiles
    // -101 dBm for 333.3 Khz singleside channel filter bandwith
    MKW01_Reg_RssiThresh, (uint_8) (0xDC) ,
    
    // ����RX��������   ��   ̽�⵽RSSI֮���  ��ʱʱ��  ���� @0x2A//
    MKW01_Reg_RxTimeout1, (uint_8) 0x00 , //disable timeout rx start
    // ����̽�⵽RSSI�������ϣ�RX PayloadReady�� ֮��� ��ʱʱ������@0x2B//
    MKW01_Reg_RxTimeout2, (uint_8) 0x00 , //disable timeout rx start
    
    // MKW01��ͷ ��preamble����С���� @0x2C-0x2D//
    MKW01_Reg_PreambleMsb, (uint_8) 0x00,
    MKW01_Reg_PreambleLsb, (uint_8) 0x10,
    
    // ����ͬ����ʶ����ƺ�ֵ����@0x2E-0x30//
    MKW01_Reg_SyncConfig, (uint_8) (SyncConfig_Sync_On
    		| SyncConfig_FifioFill_ifSyncAddres | SyncConfig_SyncSize_2) ,
    //SFD value for uncoded with phySUNMRFSKSFD = 0
    MKW01_Reg_SyncValue1, (uint_8) (0x90) ,
    //SFD value for uncoded with phySUNMRFSKSFD = 0
    MKW01_Reg_SyncValue2, (uint_8) (0x4E) ,
    
    // ���߰�ģʽ����@0x37//
    //1 �䳤����ʽ�� 2DC-free���ݰ׻��� 3CRCУ�鿪�� ��4  ��CRCУ�����ʱ���Զ�
    //���FIFO�е����ݣ�����PayloadReady�жϣ�5 ��ֹ��ַ����
    //    MKW01_Reg_PacketConfig1, 0xD8,
    //1 �䳤����ʽ�� 2DC-free���ݰ׻��� 3CRCУ�鿪�� ��4  ��CRCУ�����ʱ�Զ�
    //���FIFO�е����ݣ�������PayloadReady�жϣ�5 ��ֹ��ַ����
    //    MKW01_Reg_PacketConfig1, 0xD0,
    //1 �䳤����ʽ�� 2DC-free���ݰ׻��� 3CRCУ�鿪�� ��4  ��CRCУ�����ʱ�Զ�
    //���FIFO�е����ݣ�������PayloadReady�жϣ�5���õ�ַ���� ������ƥ��HW_ADR��
    //MKW01_Reg_PacketConfig1, 0xD2,
    //1 ��������ʽ�� 2DC-free���ݰ׻��� 3CRCУ�鿪�� ��4  ��CRCУ�����ʱ�Զ�
    //���FIFO�е����ݣ�������PayloadReady�жϣ�5���õ�ַ���� ������ƥ��HW_ADR��
    MKW01_Reg_PacketConfig1, 0x52,
    
    MKW01_Reg_NodeAdrs, 0x53,    //Ӳ�����˵�ַ
    
    // ���߰�ģʽ����@0x3D  ������ģʽǿ�ƽ������ȴ�ģʽ���ã�����AES�ӽ���//
    MKW01_Reg_PacketConfig2, 0x00,
//    // ���߰�ģʽ����@0x3D  ƥ��PARampʱ�䣬�����Զ�����RXģʽ//
//    MKW01_Reg_PacketConfig2, 0x32,
    
    //���߸��أ�payload����������@0x38//
    //֮ǰ����PacketFormat=1�����ȿɱ䣬����Ĵ�����ʾRX����󳤶ȣ�TX�в��㡣
    MKW01_Reg_PayloadLength, (uint_8) PAY_LOAD_LENGTH  //��rx����󳤶�
};

static int MACexp2(uint_8 zhishu);
//static void Old_RF_SendData(uint_8 dataLength,uint_8* data);
static void MKW01Drv_ConfigureDIOPinsFast(uint_8 regDio1, uint_8 regDio2);

static void MKW01Drv_SetOperatingMode(uint_8 mode);




//����ӿں���

//=========================================================================
//�������ƣ�RF_Init
//���ܸ�Ҫ��������ģ��Ĵ���д���ֵ
//����˵������
//�������أ���
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
    //���ñ�����
    MKW01Drv_WriteRegister(MKW01_Reg_BitrateMsb,
    		                           (uint_8) ( Bitrate_150000 >> 8  ) );
    MKW01Drv_WriteRegister(MKW01_Reg_BitrateLsb,
    		                         (uint_8) ( Bitrate_150000 & 0x00FF) );
    //����Ƶ��ƫ��
    MKW01Drv_WriteRegister(MKW01_Reg_FdevMsb, (uint_8) (Fdev_170000 >> 8));
    MKW01Drv_WriteRegister(MKW01_Reg_FdevLsb,
    		                           (uint_8) ( Fdev_170000 & 0x00FF ) );

    MKW01Drv_WriteRegister(MKW01_Reg_RxBw, (DccFreq_2 | RxBw_333300));
    MKW01Drv_WriteRegister(MKW01_Reg_AfcBw, (DccFreq_2 | RxBw_333300));

    MKW01Drv_ConfigureDIOPinsFast((DIO0_RxPayloadReady | DIO1_RxFifoLevel),
    		                             (DIO4_RxTimeout | DIO5_RxClkOut));
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; // ���־Ϊ
    PORTC_PCR4 |= PORT_PCR_IRQC(9);  // �����������ж�
    NVIC_ICPR |= 0x80000000;         // ����ʹ��D���ж�
    NVIC_ISER |= 0x80000000;
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
}


//=========================================================================
//�������ƣ�RF_SendData
//���ܸ�Ҫ��RFģ�鷢�ͺ���
//����˵����dataLength:��Ч���ݳ��ȣ�data:��������������
//�������أ���
//=========================================================================
void RF_SendData(uint_8 dataLength,uint_8* data,uint_8 HW_ADR)
{
    uint_8 blockIndex;
    uint_8 x1;
    uint_8 checkNum = 0;
    //Խ�緵�أ���2����Ϊ���а��������ֽ����ַ�ֽڣ�
    if(dataLength >= (PAY_LOAD_LENGTH - 2) )
        return;

    //1.׼������
    NVIC_ICER = 0x80000000;//��ֹ DIO0,DIO1 and DIO4 �ж�
    //���շ���ǿ���л���StandBy״̬
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    //��FIFONotEmpty�ǿ�ʱ����FIFO
    while(MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40)
        (void)MKW01Drv_ReadRegister(MKW01_Reg_Fifo);
    //2.����FIFO��ֵ�ͷ��Ϳ�ʼ������FIFO�е������ֽ�������fifoThreshold�ŷ��Ͱ�
    MKW01Drv_WriteRegister(MKW01_Reg_FifoThresh,
    		                        ((uint_8)(PAY_LOAD_LENGTH -1) & 0x7F));

    x1 = IntDisableAll();

    //3.���FIFO��2�ֽڣ�
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, HW_ADR);       //���Ӳ�����˵�ַ
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, dataLength);   //�����Ч�����ֽ�
    checkNum = HW_ADR + dataLength; //����У���
    for(blockIndex = 0;blockIndex < dataLength; blockIndex++)
    {
    	//�����Ч����
        WAIT_SPITX_BUFFER_EMPTY;
        MKW01Drv_WriteRegister(MKW01_Reg_Fifo, data[blockIndex]);

        checkNum += data[blockIndex];//����У���
    }
    for( ;blockIndex < PAY_LOAD_LENGTH-4 ; blockIndex++)
    {
    	//�����Ч���ݣ�������䣩
        WAIT_SPITX_BUFFER_EMPTY;
        MKW01Drv_WriteRegister(MKW01_Reg_Fifo, 0);
    }

    //4.���һ�ֽ�У����뱣��һ�����ݣ�2�ֽڣ���
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, checkNum);
    MKW01Drv_WriteRegister(MKW01_Reg_Fifo, 0);

    IntRestoreAll(x1) ;
    //5.��������DIO����
    //MKW01Drv_ConfigureDIOPins((DIO0_TxPacketSent | DIO1_TxFifoLevel
    //  | DIO2_TxLowBat | DIO3_TxLowBat), (DIO4_TxLowBat | DIO5_TxClkOut));

    //TX should be enabled until this line, and the Interrupt Flag set.
    MKW01Drv_WriteRegister(MKW01_Reg_OpMode, (uint_8) ( OpMode_Sequencer_On
    		                    | OpMode_Listen_Off | OpMode_Transmitter));
    Delay_ms(5);  //���������2015-8-13
    //��FIFONotEmpty�ǿ�ʱ���ȴ�
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
//�������ƣ�RFSendDataByCSMACA
//���ܸ�Ҫ��RF����CSMA/CA�㷨����һ֡
//����˵����DataAdr--Ҫ�������ݵ��׵�ַ;DataLen--Ҫ���������ֽ���
//�������أ���
//�ڲ����ã��������RFEnergyDetect
//����˵�����������в���CSMA/CA�㷨����������ʱһ�����(2^BE��Χ�� BE=minBE)��ʱ�䣬
//         Ȼ���ж��ŵ��Ƿ���У�������з��ͣ���������������ʱ������ʱ��(BE++,
//         ��BE��maxBE֮����ʱ)������ѭ��(NB<maxNB)��,ά����������BE��NB��
//         BE��ʼ��ΪminBE��ÿ����ʱ������(2^BE-1)�ķ�Χ�������ʱһ��ʱ�䡣���ŵ�
//         ������BE=min(BE+1,maxBE)��NBΪѭ������
//         maxNB--CSMA/CA�㷨�У���ʾ��ೢ�Է��͵Ĵ������������Ƿ���ʧ���ˣ�
//         ��ô��ǿ�Ʒ���һ��
//         maxBE--CSMA/CA�㷨�У���ʾ�˱�ָ�������ֵ
//=========================================================================
void RFSendDataByCSMACA(uint_8 DataLen, uint_8 *DataAdr,uint_8 isEnergyDetect,uint_8 HW_ADR)
{
	uint_8 maxNB,maxBE;
	uint_8 NB,BE;
	uint_32 i,j;
	//
	maxNB=50;     //
	maxBE=6;      //
	NB = 0;       //NB��ʾ��ǰ���Է��͵Ĵ���
	BE = 1;       //BE����ָ������˱ܵ�ָ��ֵ������˱ܵ�ʱ��ѡ����2^BE��0֮��

    for(NB =0; NB < maxNB; NB++)
    {
        //����ŵ��Ƿ���У�������з�������
        i=0x85;    //�������������һ����ֵ
        //�����Ҫ�������ֵ��isEnergyDetectֵΪ1
    	if(1==isEnergyDetect)   i = RFEnergyDetect();
        //�������ֵ����AD����ô���з���  �ֲ��Ƽ�ֵΪ0x52��Ӧ����-82dBM
        if(i >= ENERGY_SEND_MIN) break;
    	//��(0-2^BE)��Χ������ȴ�һ��ʱ��
        //����ŵ������У�����BE���ӣ����BE���趨�����ֵ������ô����Ϊ���ֵ
        BE = (BE < maxBE) ? (BE + 1) : maxBE;
        j = rand() % MACexp2(BE)+1;
        Delay_ms(j);
        //��������һ������������ӣ�Ҳ���Ǹ������������ϴε����ֵ(��ʱ��ʵ��)
    }
    //����
    RF_SendData(DataLen,DataAdr,HW_ADR);
}

//------------------�ڲ����ú�������--------------------------------------
//=========================================================================
//�������ƣ�MKW01Drv_ConfigureDIOPinsFast
//���ܸ�Ҫ������DIO����ģʽ
//����˵����regDio1:DIOMapping1�Ĵ���ֵ
//         regDio2��DIOMapping2�Ĵ���ֵ
//�������أ���
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
//�������ƣ�MKW01Drv_SetOperatingMode
//���ܸ�Ҫ������RF����ģʽ
//����˵����mode:RF����ģʽ
//�������أ���
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
//------------------�ڲ����ú�����������--------------------------------------



//=========================================================================
//�������ƣ�MKW01Drv_IRQ_DIO1_Enable
//���ܸ�Ҫ��ʹ��IRQ1
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_IRQ_DIO1_Enable(void)
{
	// TODO! Need to enable Sensitivity parameter
    PORTC_PCR4 |= PORT_PCR_ISF_MASK;   // ���жϱ�־λ
    PORTC_PCR4 |= PORT_PCR_IRQC(9);    // �����ж�����
}



//û��



//=========================================================================
//�������ƣ�RFEnergyDetect
//���ܸ�Ҫ����ȡRSSIֵ
//����˵����ע�⣬���ص�RSSIֵΪ�����ֵ���û���ʹ��ʱӦȡ�������纯������5��
//         ��ʵ��RSSIֵΪ-5
//�������أ���
//=========================================================================
uint_8 RFEnergyDetect(void)
{
    uint_8 Rssi = 0;
    //���շ�����Ϊ����ģʽ��֮���ٴ�FIFO�ж������յ������ݣ���֤�����ж��ߣ�
    MKW01Drv_SetOperatingMode(OpMode_StandBy);

    //�ȴ�״̬ת����ɣ���GZF 2015-1-16 ��� ��
//    while ((MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags1)
//    		& IrqFlags1_ModeReady) == 0x00);
    Rssi = MKW01Drv_ReadRegister(MKW01_Reg_RssiValue) / 2;
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
    //�ȴ�״̬ת�����,��GZF 2015-1-16 ��� ��
//    while ((MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags1)
//    		& IrqFlags1_ModeReady) == 0x00);
    return Rssi;
}
//=========================================================================
//�������ƣ�RFTempDetect
//���ܸ�Ҫ����ȡrf�¶�ֵ
//����˵������
//�������أ���
//=========================================================================
uint_8 RFTempDetect(void)
{
    uint_8 tmpI=0;
    //1)��MKW01_Reg_Temp1�ĵ�3λ����Ϊ1�������¶Ȳ���
    MKW01Drv_SetOperatingMode(OpMode_StandBy);

    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    tmpI=tmpI|(1<<3);
    MKW01Drv_WriteRegister(MKW01_Reg_Temp1,tmpI);
    //2)�ж�MKW01_Reg_Temp1�ĵ�2λ��=1:���ڲ����¶ȣ�
    //  =0:�������,�¶�ֵ�����MKW01_Reg_Temp2
    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    //uart_send1(UART_0,tmpI);
    while((tmpI&(1<<2))!=0)
    {
    	tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp1);
    	//uart_send1(UART_0,tmpI);
    }//�ȴ��������

    tmpI=MKW01Drv_ReadRegister(MKW01_Reg_Temp2);
    MKW01Drv_SetOperatingMode(OpMode_Receiver);
    return tmpI;
}

//=========================================================================
//�������ƣ�RF_ReceiveFrame
//���ܸ�Ҫ��RFģ�����һ֡
//����˵����pbuf�����������׵�ַ;plen:�������ݳ�����Чָ��
//         ���������ݳ���ReDataLen<=60�ֽڣ�
//         ���Ӵ���һ���ֽڣ�����ֵ=pbuf[ReDataLen]
//�������أ�����״̬��־λ��=0,����������=����ֵ�������쳣��
//��          ע�� 20141126 WYH ��
//=========================================================================
uint_8 RF_ReceiveFrame(uint_8* pbuf, uint_8* plen,uint_8 HW_ADR )
{
	uint_8 flag;
    uint_8 Rssi;
    uint_8 checkadr;
    uint_8 checkNum ;
    uint_8 ReDataLen;
    uint_32 i;

    //1. ��ʱ������ʼ��
    flag = 0;
    Rssi = 0;
    checkadr=0;
    checkNum = 0;
    ReDataLen=0;

    Delay_ms(18);    //�����������2015-8-13

    //2. ���շ�����Ϊ����ģʽ��֮���ٴ�FIFO�ж������յ������ݣ���֤�����ж��ߣ�
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    Rssi = MKW01Drv_ReadRegister(MKW01_Reg_RssiValue) / 2;  //����ֵ

    //3. ����Ӳ�����˵�ַ��1�ֽڣ�,�����е�ַУ��
    //����Ӳ�����˵�ַ
    MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, checkadr);
    checkNum += checkadr;                            //����У���
    if(checkadr != HW_ADR) goto RF_ReceiveFrame_err; //Ӳ����ַУ�����

    //4. Ӳ����ַУ����ȷ����������һ�ֽڣ���Ϊ��������֡��Ч���ȣ�
    //������������֡��Ч�����ֽ�
    MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, ReDataLen);
    checkNum += ReDataLen;                           //����У���

    //5. �������ݣ�PAY_LOAD_LENGTH-4���ֽڣ�
    i=0;
    //FIFO�ǿ������е����ݶ���
    while( (MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40) )
    {
    	//�����ݶ���pbuf[i]��
        MKW01Drv_ReadRegisterFastMacro(MKW01_Reg_Fifo, pbuf[i]);
        if(i < (PAY_LOAD_LENGTH - 4)) checkNum += pbuf[i];//��������У���
        i++;
    }
    //6. pbuf[PAY_LOAD_LENGTH - 4]�ֽ��ǽ�������У��ͣ��ۼ�У�飩
    //7. �жϽ��ո����Լ�У����Ƿ���ȷ
    if (!(i == (PAY_LOAD_LENGTH - 2)
    		                     && (checkNum == pbuf[PAY_LOAD_LENGTH-4])))
    	goto RF_ReceiveFrame_err;           //���ո�����У��ʹ���
    //8. ������ȷ
    	 flag = 0;          //���ำֵ
    	 *plen = ReDataLen;   //���ؽ���֡�ĳ���

//        //20141124 ��� �ж���֡
//		for(j=0;j<(PAY_LOAD_LENGTH - 4);j++)
//		{
//			if (rf_pre_redata[j] != pbuf[j]) break;
//		}
//		if(j==PAY_LOAD_LENGTH - 4) goto RF_ReceiveFrame_err;   //
//	   	for(j=0;j<(PAY_LOAD_LENGTH - 4);j++) rf_pre_redata[j] = pbuf[j];
	   	//��pbuf[ReDataLen]�д�����������ֵ
	   	pbuf[ReDataLen]=Rssi;
	   	goto RF_ReceiveFrame_exit;

		RF_ReceiveFrame_err:
				flag = 1;            //����ʧ�ܱ�־λ
		RF_ReceiveFrame_exit:
		//��FIFO
		while(MKW01Drv_ReadRegister(MKW01_Reg_IrqFlags2) & 0x40)
			   (void)MKW01Drv_ReadRegister(MKW01_Reg_Fifo);
		//��������Ϊ����ģʽ
		MKW01Drv_SetOperatingMode(OpMode_Receiver);
		return flag;
}

//=========================================================================
//�������ƣ�MKW01xDrv_IRQ_PortConfig
//���ܸ�Ҫ������MCU��Ӧ������������ģ��DIO�ϵ��ж�
//����˵������
//�������أ���
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
//�������ƣ�MKW01Drv_ConfigureDIOPins
//���ܸ�Ҫ������DIO���ţ��Ա���MCU�˽�����������շ�����״̬
//����˵����regDio1��DIO1�Ĵ�����regDio2��DIO2�Ĵ���
//�������أ���
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
//�������ƣ�MKW01Drv_RadioStandByReq
//���ܸ�Ҫ����RF��ΪStandByģʽ
//����˵������
//�������أ���
//=========================================================================
uint_8 MKW01Drv_RadioStandByReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
    return 0;
}

//=========================================================================
//�������ƣ�MKW01Drv_RadioSleepReq
//���ܸ�Ҫ����RF��ΪSleepģʽ
//����˵������
//�������أ���
//=========================================================================
uint_8 MKW01Drv_RadioSleepReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_Sleep);
    return 0;
}

//=========================================================================
//�������ƣ�MKW01Drv_RadioWakeUpReq
//���ܸ�Ҫ����RF��Ϊ����ģʽ
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_RadioWakeUpReq(void)
{
    MKW01Drv_SetOperatingMode(OpMode_StandBy);
}

//=========================================================================
//�������ƣ�MKW01Drv_RC_Calibration
//���ܸ�Ҫ��RC����У׼�����Ӳ���汾ΪV2a���ϵ�ʱӦ����RCУ׼//
//����˵������
//�������أ���
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
//�������ƣ�MKW01Drv_IRQ_Enable
//���ܸ�Ҫ�����������ж�����
//����˵����SensitivityDIO0���ж�����   SensitivityDIO1���ж�����
//�������أ���
//=========================================================================
void MKW01Drv_IRQ_Enable(uint_8 SensitivityDIO0, uint_8 SensitivityDIO1)
{

    PORTC_PCR3 |= PORT_PCR_ISF_MASK; // ���жϱ�־λ
    PORTC_PCR3 |= PORT_PCR_IRQC(SensitivityDIO0);  //�����ж�����
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; // ���жϱ�־λ
    PORTC_PCR4 |= PORT_PCR_IRQC(SensitivityDIO1);  //�����ж�����
}

//=========================================================================
//�������ƣ�MKW01Drv_CCA_Enable
//���ܸ�Ҫ��ʹ��CCA
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_CCA_Enable(void)
{

    PORTD_PCR4 |= PORT_PCR_ISF_MASK; // ���жϱ�־λ
    PORTD_PCR4 |= PORT_PCR_IRQC(9);  // �����ж�����
}

//=========================================================================
//�������ƣ�MKW01Drv_IRQ_Clear
//���ܸ�Ҫ����IRQ�ж�
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_IRQ_Clear(  void)
{

    PORTC_PCR3 |= PORT_PCR_ISF_MASK; //��PORTC_PCR3�ж�
    PORTC_PCR4 |= PORT_PCR_ISF_MASK; //��PORTC_PCR4�ж�
 
}

//=========================================================================
//�������ƣ�MKW01Drv_CCA_Clear
//���ܸ�Ҫ����IRQ�ж�
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_CCA_Clear(void)
{
    PORTD_PCR4 |= PORT_PCR_ISF_MASK; //��PORTD_PCR4�ж�
}

//=========================================================================
//�������ƣ�MKW01Drv_IRQ_Disable
//���ܸ�Ҫ����ֹIRQ
//����˵������
//�������أ���
//=========================================================================
void MKW01Drv_IRQ_Disable(void)
{
    PORTC_PCR3 &= ~PORT_PCR_IRQC_MASK;
    PORTC_PCR4 &= ~PORT_PCR_IRQC_MASK; 
}

//=========================================================================
//�������ƣ�MKW01Drv_IRQ_DIO1_Disable
//���ܸ�Ҫ����ֹDIO1ģ��
//����˵������
//�������أ���
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

//����ָ���͵�����������
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




