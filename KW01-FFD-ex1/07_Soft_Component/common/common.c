

#include "common.h"

static  uint_32  int_disable_level; // ��ǰ�ж�Ƕ�ײ���

//=========================================================================
//�������ƣ�init_critical
//����˵������
//�������أ���
//���ܸ�Ҫ����ʼ���ٽ������ʿ���
//=========================================================================
void  init_critical(void) 
{
    ENABLE_INTERRUPTS;
    int_disable_level = 0;
}

//=========================================================================
//�������ƣ�enter_critical
//����˵������
//�������أ���
//���ܸ�Ҫ�������ٽ���
//=========================================================================
void  enter_critical(void)
{
    if (0 == int_disable_level)
    {
        DISABLE_INTERRUPTS;
    }
    int_disable_level++;
}

//=========================================================================
//�������ƣ�exit_critical
//����˵������
//�������أ���
//���ܸ�Ҫ���뿪�ٽ���
//=========================================================================
void  exit_critical(void) 
{
    if (int_disable_level)
{
        int_disable_level--;
        if (0 == int_disable_level)
        {
            ENABLE_INTERRUPTS;
        }
    }
}

//============================================================================
//�������ƣ�enable_irq
//�������أ���  
//����˵����irq��irq��
//���ܸ�Ҫ��ʹ��irq�ж� 
//============================================================================
void enable_irq (uint_16 irq)
{
    //ȷ��irq��Ϊ��Ч��irq��
    if (irq > 32)	irq=32;
    //Re-enable PORTC and PORTD interrupts      //Same interrupt vector for PORTC and PORTD
    NVIC_ICPR |= (1<<(irq%32));
    NVIC_ISER |= (1<<(irq%32));

}

//============================================================================
//�������ƣ�disable_irq
//�������أ���      
//����˵����irq��irq��
//���ܸ�Ҫ����ֹirq�ж� 
//============================================================================
void disable_irq (uint_16 irq)
{

    //ȷ��irq��Ϊ��Ч��irq��
    if (irq > 32)	irq=32;
    
    NVIC_ICER = (1<<(irq%32));     
}
 
//============================================================================
//�������ƣ�set_irq_priority
//�������أ���      
//����˵����irq��irq��         											   
//         prio�����ȼ�
//���ܸ�Ҫ������irq�жϺ����ȼ� 
//============================================================================
void set_irq_priority (uint_16 irq, uint_16 prio)
{   
    uint_8 *prio_reg;
    uint_8 err = 0;
    uint_8 div = 0;
   
    if (irq > 32)
    {    
        err = 1;
    }
    if (prio > 3)
    {     
        err = 1;
    }
    if (err != 1)
    {  
       div = irq / 4;
       prio_reg = (uint_8 *)((uint_32)&NVIC_IP(div));
       *prio_reg = ( (prio&0x3) << (8 - ARM_INTERRUPT_LEVEL_BITS) );             
    }
}

//============================================================================
//�������ƣ�IntRestoreAll
//�������أ���      
//����˵����var���ж��ź�         	
//���ܸ�Ҫ���ָ������ж�
//============================================================================
void IntRestoreAll(uint32_t var)
{ 
	//PRIMASK�����ȼ��ж����μĴ�����Ϊ����Ĵ���
    asm(" MSR PRIMASK, %0" : : "r" (var));//��ȡvar�е�ֵ����д��PRIMASK
    asm(" CPSIE i");
}

//============================================================================
//�������ƣ�IntDisableAll
//�������أ�var����ǰ  PRIMASK�Ĵ�����ֵ
//����˵������      	
//���ܸ�Ҫ�����������ж�
//============================================================================
uint32_t IntDisableAll(void)
{
	uint32_t var;
	//PRIMASK=1:�������п��������ȼ����쳣��=0����Ӱ��
	asm("MRS  %0, PRIMASK" : "=r" (var));//��ȡPRIMASK�Ĵ�����ֵ����д��R0���������var��
	asm(" CPSID i");
	
	return var;
}

//============================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������      	
//���ܸ�Ҫ����ʱ - ΢�뼶
//============================================================================
void Delay_us(uint16_t u16us) 
{
  uint32_t u32ctr;                                          
  for(u32ctr = 0; u32ctr < ((48000 / 1000 / 11) * u16us); u32ctr++)
  {
    asm ("NOP");   
  }      
}
//============================================================================
//�������ƣ�Delay_us
//�������أ���
//����˵������      	
//���ܸ�Ҫ����ʱ - ���뼶
//============================================================================
void Delay_ms(uint16_t u16ms) 
{                            
  //@AC 1ms  
  //gSystemClock_d is given in KHz --> gSystemClock_d * 1000 /1000 = counts to have 1ms
  uint32_t u32ctr;
  for(u32ctr = 0; u32ctr < ((48000 / 10)*u16ms); u32ctr++)
  {
    asm ("NOP"); 
  }    
}
//============================================================================
//�������ƣ�Wait_6_BitClks
//�������أ���
//����˵������      	
//���ܸ�Ҫ����ʱ - 6����λ����ʱ��
//============================================================================
void Wait_6_BitClks(void) // ����ʱ�� : BRA/BSR/JSR to here: 3/5/6 mcuCycles    
{                         //         2xNOP+RTS          :     8 mcuCycles
  asm ("nop");
  asm ("nop");
}

//============================================================================
//�������ƣ�ftochar
//�������أ���
//����˵����f:��Ҫת���ĸ�����        c[]:��������ת������ַ���
//���ܸ�Ҫ����������ת��Ϊ�ַ���������һλС����
//============================================================================
void ftochar(float f,uint_8 c[])
{
	uint_16 p,q,i,j,a[5]={0},b[5]={0};
	p=(uint_16)f;//��������
	q=(f-p)*10+0.5;//С�����֣�����һλС�����������룩
	i=0;
	if(0==p)//��������Ϊ0
		c[i++]='0';
	else//�������ֲ�Ϊ0
	{
		j=0;
		while(p!=0)//���������
		{
			a[j++]=p%10;
			p/=10;
		}
		while(j>0)//����������תΪ���򣬲�ת��Ϊ��Ӧ���ַ���ʽ
		{
			c[i++]=a[j-1]+'0';
			--j;
		}
	}
	c[i++]='.';//С����
	if(0==q)//С������Ϊ0
		c[i++]='0';
	else//С�����ֲ�Ϊ0
	{
		j=0;
		while(q!=0)//���������
		{
			b[j++]=q%10;
			q/=10;
		}
		while(j>0)//����������תΪ���򣬲�ת��Ϊ��Ӧ���ַ���ʽ
		{
			c[i++]=b[j-1]+'0';
			--j;
		}
	}
	c[i]='\0';
}


