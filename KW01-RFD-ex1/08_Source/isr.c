//===========================================================================
//文件名称：isr.c（ 中断底层驱动构件源文件）
//功能概要：中断服务例程存放处
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：根据实际情况更新
//===========================================================================
#include "includes.h"
#include "01_app_include.h"

//内部函数声明
uint8_t CreateFrame(uint8_t Data,uint8_t * buffer);


void isr_uart0_re(pointer user_isr_ptr)
{
	uint_8 c;
	DISABLE_INTERRUPTS;                //关总中断
	//-----------------------------------------------------------------------

    uart_re1(UART_0, &c);             //接收一个字节

    //调用内部函数CreateFrame进行组帧
    if(CreateFrame(c,g_uart_recvBuf)!=0)    //组帧成功
    {
   		_lwevent_set(&lwevent_group,EVENT_UART0_RE);
    }

    //-----------------------------------------------------------------------
	ENABLE_INTERRUPTS;                //开总中断
}


//===========================================================================
//ISR 名称：gpio_CD_ISR中断服务例程
//功能概要：gpio 中断例程，判断DIO0、DIO1、DIO4产生的中断
//===========================================================================
void gpio_CD_ISR(pointer user_isr_ptr)
{
//	DISABLE_INTERRUPTS;           //关总中断
	//-----------------------------------------------------------------------

	//Delay_ms(20);

//	DEBUG_LOG(10,"==RF_ISR==")
	if((PORTC_PCR4 & PORT_PCR_ISF_MASK)) 	//DIO1中断
	{
		_mqx_uint recv_msg_temp[RECV_MSG_SIZE];
		//接收数据包成功，置事件位EVENT_RF_RECV，启动task_rf_recv任务

		if(0 == RF_ReceiveFrame(recv_msg_temp,&g_rfRecCount,HD_adr))
		{
			uint_8 res1 = _lwmsgq_send((pointer)recv_queue,recv_msg_temp,0);
			switch (res1) {
				case LWMSGQ_INVALID:
					DEBUG_LOG(25,"The handle  was not valid.");
					break;
				case LWMSGQ_FULL:
					DEBUG_LOG(9,"Fuck full");
					break;
				case MQX_CANNOT_CALL_FUNCTION_FROM_ISR:
					DEBUG_LOG(9,"Fuck ISR!");
					break;
				default:
					break;
			}

		}
			PORTC_PCR4 |= PORT_PCR_ISF_MASK; 	        //清标志位
	}
	//-----------------------------------------------------------------------
	UART0_S1|=0x1F;
//    ENABLE_INTERRUPTS;                                  //开总中断
}

//内部调用函数

//===========================================================================
//ISR名称：Createbuffer
//功能概要：组建数据帧，将待组帧数据加入到数据帧中
//参数说明：Data：                            待组帧数据
//          buffer:        数据帧变量
//函数返回：组帧状态    0-组帧未成功，1-组帧成功
//备注：十六进制数据帧格式
//               帧头        + 数据长度          + 有效数据    +  帧尾
//        FrameHead   +   len   + 有效数据    + FrameTail
//===========================================================================

#define FrameHead    (0x05)         //帧头
#define FrameTail    (0x03)         //帧尾
uint8_t CreateFrame(uint8_t Data,uint8_t * buffer)
{
    static uint8_t frameLen=0;    //组帧计数器
    uint8_t frameFlag;              //组帧状态

    frameFlag=0;            //组帧状态 初始化
    //根据静态变量frameCount组帧
    switch(frameLen)
    {
        case 0:    //第一个数据
        {
            if (Data==FrameHead)    //收到数据是帧头FrameHead
            {
                buffer[0]=Data;
                frameLen++;
                frameFlag=0;        //组帧开始
            }
            break;
        }
        case 1:    //第二个数据，该数据是随后接收的数据个数
        {
            buffer[1]=Data;
            frameLen++;
            break;
        }
        default:    //其他情况
        {
            //第二位数据是有效数据长度,根据它接收余下的数据直到帧尾前一位
            if(frameLen>=2 && frameLen<=(buffer[1] + 1))
            {
                buffer[frameLen]=Data;
                frameLen++;
                break;
            }

            //若是末尾数据则执行
            if(frameLen>=(buffer[1]+2))
            {
                if (Data==FrameTail)    //若是帧尾
                {
                    buffer[frameLen]=Data;     //将帧尾存入缓冲区
                    frameFlag=1;    //组帧成功
                }
                frameLen=0;     //计数清0，准备重新组帧
                break;
            }
        }
    }     //switch_END
    return frameFlag;                 //返回组帧状态
}
