//===========================================================================
//�ļ����ƣ�isr.c�� �жϵײ���������Դ�ļ���
//���ܸ�Ҫ���жϷ������̴�Ŵ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼������ʵ���������
//===========================================================================
#include "includes.h"
#include "01_app_include.h"

//�ڲ���������
uint8_t CreateFrame(uint8_t Data,uint8_t * buffer);


void isr_uart0_re(pointer user_isr_ptr)
{
	uint_8 c;
	DISABLE_INTERRUPTS;                //�����ж�
	//-----------------------------------------------------------------------

    uart_re1(UART_0, &c);             //����һ���ֽ�

    //�����ڲ�����CreateFrame������֡
    if(CreateFrame(c,g_uart_recvBuf)!=0)    //��֡�ɹ�
    {
   		_lwevent_set(&lwevent_group,EVENT_UART0_RE);
    }

    //-----------------------------------------------------------------------
	ENABLE_INTERRUPTS;                //�����ж�
}


//===========================================================================
//ISR ���ƣ�gpio_CD_ISR�жϷ�������
//���ܸ�Ҫ��gpio �ж����̣��ж�DIO0��DIO1��DIO4�������ж�
//===========================================================================
void gpio_CD_ISR(pointer user_isr_ptr)
{
	DISABLE_INTERRUPTS;           //�����ж�
	//-----------------------------------------------------------------------

	//Delay_ms(20);


	if((PORTC_PCR4 & PORT_PCR_ISF_MASK)) 	//DIO1�ж�
	{
		//�������ݰ��ɹ������¼�λEVENT_RF_RECV������task_rf_recv����
		if(0 == RF_ReceiveFrame(rf_recvBuf,&g_rfRecCount,HD_adr))
		{
			_lwevent_set(&lwevent_group,EVENT_RF_RECV);	//��RF�����¼�λ������RF��������
		}
			PORTC_PCR4 |= PORT_PCR_ISF_MASK; 	        //���־λ
	}
	//-----------------------------------------------------------------------
	UART0_S1|=0x1F;
    ENABLE_INTERRUPTS;                                  //�����ж�
}

//�ڲ����ú���

//===========================================================================
//ISR���ƣ�Createbuffer
//���ܸ�Ҫ���齨����֡��������֡���ݼ��뵽����֡��
//����˵����Data��                            ����֡����
//          buffer:        ����֡����
//�������أ���֡״̬    0-��֡δ�ɹ���1-��֡�ɹ�
//��ע��ʮ����������֡��ʽ
//               ֡ͷ        + ���ݳ���          + ��Ч����    +  ֡β
//        FrameHead   +   len   + ��Ч����    + FrameTail
//===========================================================================

#define FrameHead    (0x05)         //֡ͷ
#define FrameTail    (0x03)         //֡β
uint8_t CreateFrame(uint8_t Data,uint8_t * buffer)
{
    static uint8_t frameLen=0;    //��֡������
    uint8_t frameFlag;              //��֡״̬

    frameFlag=0;            //��֡״̬ ��ʼ��
    //���ݾ�̬����frameCount��֡
    switch(frameLen)
    {
        case 0:    //��һ������
        {
            if (Data==FrameHead)    //�յ�������֡ͷFrameHead
            {
                buffer[0]=Data;
                frameLen++;
                frameFlag=0;        //��֡��ʼ
            }
            break;
        }
        case 1:    //�ڶ������ݣ��������������յ����ݸ���
        {
            buffer[1]=Data;
            frameLen++;
            break;
        }
        default:    //�������
        {
            //�ڶ�λ��������Ч���ݳ���,�������������µ�����ֱ��֡βǰһλ
            if(frameLen>=2 && frameLen<=(buffer[1] + 1))
            {
                buffer[frameLen]=Data;
                frameLen++;
                break;
            }

            //����ĩβ������ִ��
            if(frameLen>=(buffer[1]+2))
            {
                if (Data==FrameTail)    //����֡β
                {
                    buffer[frameLen]=Data;     //��֡β���뻺����
                    frameFlag=1;    //��֡�ɹ�
                }
                frameLen=0;     //������0��׼��������֡
                break;
            }
        }
    }     //switch_END
    return frameFlag;                 //������֡״̬
}