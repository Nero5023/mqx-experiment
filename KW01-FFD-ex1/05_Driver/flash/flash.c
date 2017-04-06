//文件名称：flash.c
//功能概要：flash底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版    本：V3.0(WYH),20140909
//包含头文件
#include "flash.h" 

// Flash控制器宏定义 ，内部使用 
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)


//Flash命令宏定义，内部使用
#define RD1SEC    0x01   // 读整个扇区
#define PGMCHK    0x02   // 写入检查
#define RDRSRC    0x03   // 读目标数据
#define PGM4      0x06   // 写入长字
#define ERSBLK    0x08   // 擦除整块Flash
#define ERSSCR    0x09   // 擦除Flash扇区
#define RD1ALL    0x40   // 读所有的块
#define RDONCE    0x41   // 只读一次
#define PGMONCE   0x43   // 只写一次
#define ERSALL    0x44   // 擦除所有块
#define VFYKEY    0x45   // 验证后门访问钥匙

//===========================================================================
//函数名称：flash_cmd_launch
//函数返回：0-成功 1-失败
//参数说明：无
//功能概要：启动Flash命令
//===========================================================================
static uint_8 flash_cmd_launch(void)
{
    uint_8 ret = 0;
    
        //清除访问错误标志位和非法访问标志位
    FTFA_FSTAT = ACCERR | FPVIOL;

    // 启动命令
    FTFA_FSTAT = CCIF;

    // 等待命令结束
    while(!(FTFA_FSTAT & CCIF));

    // 检查错误标志
    if(FTFA_FSTAT & (ACCERR | FPVIOL | MGSTAT0))
        ret = 1 ; //执行命令出错
        
    return ret;
}

//==========================================================================
//函数名称：flash_init
//函数返回：无
//参数说明：无
//功能概要：初始化flash模块
//==========================================================================
void flash_init(void)
{   
    // 等待命令完成
    while(!(FTFA_FSTAT & CCIF));
    
      // 清除访问出错标志位
    FTFA_FSTAT = ACCERR | FPVIOL;
    //置杂项模块中平台控制寄存器的PLACR_ESFC，Flash模块擦写保护
    BSET(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);
    //清杂项模块中平台控制寄存器的PLACR_ESFC，Flash模块擦写不保护
    //BCLR(MCM_PLACR_ESFC_SHIFT,MCM_PLACR);   //（实验观察现象用）
}

//==========================================================================
//函数名称：flash_erase
//函数返回：函数执行执行状态：0=正常；非0=异常。
//参数说明：sectorNo：目标扇区号(0~127)
//功能概要：擦除指定flash扇区
//==========================================================================
uint_8 flash_erase(uint_16 sectNo)
{
    union
    {
        uint_32  word;
        uint_8   byte[4];
    }dest;
    //扇区号检测，超过127越界
    if( sectNo > 127) 
          return 1;     //擦除命令错误
          
    dest.word = sectNo;
        dest.word <<= 10;

    //设置擦除命令
    FTFA_FCCOB0 = ERSSCR; // 擦除扇区命令
    
    //设置目标地址
    FTFA_FCCOB1 = dest.byte[2];
    FTFA_FCCOB2 = dest.byte[1];
    FTFA_FCCOB3 = dest.byte[0];
    
    // 执行命令序列
    if(flash_cmd_launch() != 0)    //若执行命令出现错误
        return 1;     //擦除命令错误
   
    // 若擦除sector0时，则解锁设备
    if(dest.word <= 0x800)
    {
        // 写入4字节
        FTFA_FCCOB0 = PGM4; 
        // 设置目标地址
        FTFA_FCCOB1 = 0x00;
        FTFA_FCCOB2 = 0x04;
        FTFA_FCCOB3 = 0x0C;
        // 数据
        FTFA_FCCOB4 = 0xFF;
        FTFA_FCCOB5 = 0xFF;
        FTFA_FCCOB6 = 0xFF;
        FTFA_FCCOB7 = 0xFE;
        // 执行命令序列
        if(flash_cmd_launch() != 0)  //若执行命令出现错误
            return 2;   //解锁命令错误
    }
    
    return 0;  //成功返回
}

//==========================================================================
//函数名称：flash_write
//函数返回：函数执行状态：0=正常；非0=异常。
//参数说明：sectNo：目标扇区号(0~127)
//         offset：写入扇区内部偏移地址（0~1023）,要求是4的倍数
//         cnt：写入字节数目（1~1024）
//         offset+cnt<=1024
//         buf：源数据缓冲区首地址
//功能概要：flash写入操作
//==========================================================================
uint_8 flash_write(uint_16 sectNo,uint_16 offset,uint_16 cnt,uint_8 buf[])
{
    uint_32 i;
    
    union
    {
        uint_32 word;
        uint_8 byte[4];
    } dest;
    
    //参数检查
    if(offset % 4 != 0 || sectNo > 127 || cnt <= 0 || (offset+cnt) > 1024||buf == NULL)
        return 1;   
        //扇区号检测，超过127越界
    if(sectNo > 127)  return 1;     //擦除命令错误
          
    // 设置写入命令
    FTFA_FCCOB0 = PGM4;
    dest.word = sectNo;
    dest.word <<= 10;
    dest.word += offset;
    
    for(i = 0; i < cnt; i += 4, dest.word += 4, buf += 4)
    {
        //设置目标地址
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];
 
        //拷贝数据
        FTFA_FCCOB4 = buf[3];
        FTFA_FCCOB5 = buf[2];
        FTFA_FCCOB6 = buf[1];
        FTFA_FCCOB7 = buf[0];
        
        if(flash_cmd_launch() != 0)  return 2;  //写入命令错误
    }
    
    return 0;  //成功执行
}

//============================================================================
//函数名: flash_read
//函数返回：函数执行状态：0=正常；非0=异常。
//参数说明：sectNo：目标扇区号(0~127)
//         offset：读取扇区内部偏移地址（0~1023）
//         cnt：读取字节数目（1~1024）
//         offset+cnt<=1024
//         buf：读取数据缓冲区首地址
//功能概要：flash读取指定扇区操作
//============================================================================
void flash_read(uint_16 sectNo,uint_16 offset,uint_16 cnt,uint_8 buf[])
{
    uint_8* address;
//    uint_8 i;    //问题根源所在：
    uint_16 i;
    
        //参数检查
    if(cnt <= 0 || (offset+cnt) > 1024|| sectNo > 127)
        return ;
          
    address =  (uint_8 *)(sectNo * SIZE_FLASHPAGE+offset);//获得读取块号首地址

    for(i=0; i < cnt;i++)
        buf[i] = address[i];
}
//==========================================================================
//函数名称：flash_secure
//函数返回：函数执行状态：0=正常；非0=异常。
//参数说明：无
//功能概要：flash加密操作
//==========================================================================
uint_8 flash_secure(uint_8 key[])
{
    uint_8 vecdata[256];
    uint_16 i;
    uint_8* dataaddr;
    union
    {
        uint_32 word;
        uint_8 byte[4];
    }dest;

    dataaddr = (uint_8*)0x400;
    for(i = 0; i < 8; i++)
         vecdata[i] = key[i];
    vecdata[8]=0xFF;         //不设置块保护
    vecdata[9]=0xFF;         //不设置块保护
    vecdata[10]=0xFF;        //不设置块保护
    vecdata[11]=0xFF;        //不设置块保护
    vecdata[12]=0x80;        //加密芯片，设置允许后门秘钥访问
    vecdata[13]=0xFF;        //Flash选项寄存器默认设置为0xff
    vecdata[14]=0xFF;        //保留字段
    vecdata[15]=0xFF;        //保留字段

    for(i = 16; i < 256; i++)
        vecdata[i] = dataaddr[i];//复制原来Flash中的内容

    //设置写入命令
    FTFA_FCCOB0 = PGM4;
    dest.word = 0x400;
    dataaddr = vecdata;
    for(i = 0; i < 256; i += 4, dest.word += 4,dataaddr += 4)
    {
        //设置目标地址
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];

        // 拷贝数据
        FTFA_FCCOB4 = dataaddr[3];
        FTFA_FCCOB5 = dataaddr[2];
        FTFA_FCCOB6 = dataaddr[1];
        FTFA_FCCOB7 = dataaddr[0];
        if(flash_cmd_launch() != 0) 
            return 2;  //写入命令错误
    }
    
    return 0;
}

//==========================================================================
//函数名称：flash_unsecure
//函数返回：函数执行状态：0=正常；非0=异常。
//参数说明：无
//功能概要：删除秘钥，解密芯片
//==========================================================================
uint_8 flash_unsecure()
{
    uint_8 vecdata[256];
    uint_16 i;
    uint_8* dataaddr;
    union
    {
        uint_32 word;
        uint_8 byte[4];
    }dest;

    dataaddr=(uint_8*)0x400;
    for(i = 0; i < 8; i++)
        vecdata[i] = 0xFF;
    vecdata[8]=0xFF;         //不设置块保护
    vecdata[9]=0xFF;         //不设置块保护
    vecdata[10]=0xFF;        //不设置块保护
    vecdata[11]=0xFF;        //不设置块保护
    vecdata[12]=0xFE;        //解密芯片
    vecdata[13]=0xFF;        //Flash选项寄存器默认设置为0xff
    vecdata[14]=0xFF;        //保留字段
    vecdata[15]=0xFF;        //保留字段
    for(i = 16; i < 256; i++)
        vecdata[i] = dataaddr[i];//复制原来Flash中的内容

    //设置写入命令
    FTFA_FCCOB0 = PGM4;
    dest.word = 0x400;
    dataaddr = vecdata;
    for(i = 0; i < 256; i += 4, dest.word += 4, dataaddr += 4)
    {
        // 设置目标地址
        FTFA_FCCOB1 = dest.byte[2];
        FTFA_FCCOB2 = dest.byte[1];
        FTFA_FCCOB3 = dest.byte[0];

        // 拷贝数据
        FTFA_FCCOB4 = dataaddr[3];
        FTFA_FCCOB5 = dataaddr[2];
        FTFA_FCCOB6 = dataaddr[1];
        FTFA_FCCOB7 = dataaddr[0];
        if(flash_cmd_launch() != 0) 
            return 2;  //写入命令错误
    }
    
    return 0; 
}

//==========================================================================
//函数名称：flash_protect
//函数返回：无
//参数说明：无
//功能概要：flash保护操作
//==========================================================================
void flash_protect()
{
    BCLR(2,FTFA_FPROT3);
}


