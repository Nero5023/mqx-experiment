//===========================================================================
//文件名称：gpio.h
//功能概要：GPIO底层驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版        本：V3.0(WYH),20140909
//备        注：
//   (1)KW01中可供用户使用的GPIO引脚,共19个：
//      PORTA：1、2、4、19
//      PORTB：0、1、2、17
//      PORTC：1、2
//      PORTD：5、6、7
//      PORTE：0、1、16、17、18、19
//   (2)关于GPIO类中断
//      PORTA、PORTC、PORTD口引脚具有GPIO端口引脚中断功能，
//      分别是46、47号中断向量，后两者共用一个中断向量。
//   (3)以上所有引脚都具有复用功能，任何时候只能使用其中一种功能；以上引脚，复用字段全部为1
//   (4)关于本驱动的可复用性：本驱动适用于KL、KW01系列，具体芯片只需改动
//      本备注的(1)和(2)的硬件说明，其他不需改动。
//   (5)KW01中PORTB0-1,PORTD6-7具有高电流驱动能力，
//      2.7V~3.6V供电时 驱动电流20mA；1.8V~2.7V供电时 驱动电流10mA
//===========================================================================

#ifndef _GPIO_H        //防止重复定义（_GPIO_H  开头)
#define _GPIO_H

#include "common.h"   //包含公共要素头文件

// 端口号地址偏移量宏定义
#define PORTA            (0<<8)
#define PORTB            (1<<8)
#define PORTC            (2<<8)
#define PORTD            (3<<8)
#define PORTE            (4<<8)
//引脚方向宏定义
#define GPIO_IN          0
#define GPIO_OUT         1
//引脚中断类型宏定义
#define LOW_LEVEL        8    //低电平触发
#define HIGH_LEVEL       12   //高电平触发
#define RISING_EDGE      9    //上升沿触发
#define FALLING_EDGE     10   //下降沿触发
#define DOUBLE_EDGE      11   //双边沿触发

//===========================================================================
//函数名称：gpio_init
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//          dir：引脚方向（0=输入，1=输出,可用引脚方向宏定义）
//          state：端口引脚初始状态（0=低电平，1=高电平）
//功能概要：初始化指定端口引脚作为GPIO引脚功能，并定义为输入或输出，若是输出，
//          还指定初始状态是低电平或高电平
//===========================================================================
void gpio_init(uint_16 port_pin, uint_8 dir, uint_8 state);

//===========================================================================
//函数名称：gpio_set
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//          state：希望设置的端口引脚状态（0=低电平，1=高电平）
//功能概要：当指定端口引脚被定义为GPIO功能且为输出时，本函数设定引脚状态
//===========================================================================
void gpio_set(uint_16 port_pin, uint_8 state);

//===========================================================================
//函数名称：gpio_get
//函数返回：指定端口引脚的状态（1或0）
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数获取指定引脚状态
//===========================================================================
uint_8 gpio_get(uint_16 port_pin);

//===========================================================================
//函数名称：gpio_reverse
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输出时，本函数反转引脚状态
//===========================================================================
void gpio_reverse(uint_16 port_pin);

//===========================================================================
//函数名称：gpio_pull
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//          pullselect：下拉/上拉（0=下拉，1=上拉）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数设置引脚下拉/上拉
//===========================================================================
void gpio_pull(uint_16 port_pin, uint_8 pullselect);

//===========================================================================
//函数名称：gpio_enable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//          irqtype：引脚中断类型，由宏定义给出，再次列举如下：
//                  LOW_LEVEL        8    低电平触发
//                  HIGH_LEVEL       12   高电平触发
//                  RISING_EDGE      9    上升沿触发
//                  FALLING_EDGE     10   下降沿触发
//                  DOUBLE_EDGE      11   双边沿触发
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数开启引脚中断，并
//          设置中断触发条件。
//注    意：KW01芯片，只有PTA、PTC、PTD口具有GPIO类中断功能
//===========================================================================
void gpio_enable_int(uint_16 port_pin,uint_8 irqtype);

//===========================================================================
//函数名称：gpio_disable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数关闭引脚中断
//注    意：KW01芯片，只有PTA、PTC、PTD口具有GPIO类中断功能
//===========================================================================
void gpio_disable_int(uint_16 port_pin);


#endif     //防止重复定义（_GPIO_H 结尾)


//=============================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学飞思卡尔嵌入式中心（苏州华祥信息科技有限公司）
//技术咨询：0512-65214835  http://sumcu.suda.edu.cn
//业务咨询：0512-87661670,18915522016  http://www.hxtek.com.cn
