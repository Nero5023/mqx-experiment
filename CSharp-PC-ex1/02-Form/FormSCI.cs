using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using WinFormAnimation;

namespace SerialPort
{
    ///----------------------------------------------------------------------
    ///FrmSCI  :串口测试工程                                                
    ///功能描述:测试串口通信是否正常                                        
    ///         发送时可以选择字符串、十进制、十六进制三种方式              
    ///         接收到的数据分别以字符串、十进制、十六进制三种方式显示      
    ///目    的:测试串口                                                    
    ///说    明:                                                           
    ///注    意:                                                           
    ///日    期:2010年3月31日                                              
    ///编 程 者:LCX-WYH                                                     
    ///-----------串口测试工程(苏州大学飞思卡尔嵌入式中心)----------------------
    public partial class FrmSCI : Form
    {
        //委托,将从串口接收到的数据显示到接收框里面
        delegate void handleinterfaceupdatedelegate(Object textbox,
                                                    string text);
        //串口默认情况
        private string msg = "无校验,8位数据位,1位停止位(No parity,8 data " +
                            "bits,1 stop bit)";
        private string str = "串口号(Serial Port Number)、波特率(Baud Rate):";
          
        SCI sci;    //要调用SCI类中所定义的函数

        private class BarWithPic{
            public PictureBox pic;
            public ProgressBar bar;
            public bool isMonitored;
            public BarWithPic(PictureBox pic , ProgressBar bar)
            {
                this.pic = pic;
                this.bar = bar;
                isMonitored = false;
            }

        }

        private int totalLength = 0;
        private _02_Form.ProcessBarForm pgb;
        public delegate void ChangeEventHandler(object s, EventArgs e);
        public event ChangeEventHandler Changed;

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 函 数 名:FrmSCI:类FrmSCI的构造函数                              
        /// 功    能:完成窗体的初始化工作                                   
        /// 函数调用:InitializeComponent                                    
        /// </summary>                                                      
        ///-----------------------------------------------------------------
        public FrmSCI()
        {
            InitializeComponent();
            MyBarWithPic[0] = new BarWithPic(this.pictureBox1, this.progressBar7);
            MyBarWithPic[1] = new BarWithPic(this.pictureBox2, this.progressBar2);
            MyBarWithPic[2] = new BarWithPic(this.pictureBox3, this.progressBar3);
            MyBarWithPic[3] = new BarWithPic(this.pictureBox4, this.progressBar4);
            MyBarWithPic[4] = new BarWithPic(this.pictureBox5, this.progressBar5);
            MyBarWithPic[5] = new BarWithPic(this.pictureBox6, this.progressBar1);


           pb = new PictureBox();
           pic_form = new Form();
           pic_form.Controls.Add(pb);

        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:FrmSCI:窗体                                            
        /// 事    件:Load                                                   
        /// 功    能:执行加载窗体程序,自动获得串口号                        
        ///          同时在标签LblSCI中显示串口相关信息                     
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        /// <remarks></remarks>                                             
        ///-----------------------------------------------------------------
        private void FrmSCI_Load(object sender, EventArgs e)
        {
            //初始化时,按钮显示"打开串口(Open SCI)"
            this.BtnSCISwitch.Text = "打开串口(Open SCI)";
            this.CbSCIBaud.Enabled = true;    //[波特率选择框]处于可用状态
            this.CbSCIComNum.Enabled = true;　//[串口选择框]处于可用状态
            this.BtnSCISend.Enabled = false;  //发送按钮处于不可用状态
            CheckForIllegalCrossThreadCalls = false;
            //自动搜索串口,并将其加入到[串口选择框]中
            int i;
            string[] SCIPorts;
            SCIPorts = SCI.SCIGetPorts();

            this.CbSCIComNum.Items.Clear();//首先将现有的项清除掉
            for(i = 0; i < SCIPorts.Length; i++)
                //向[串口选择框]中添加搜索到的串口号
                this.CbSCIComNum.Items.Add(SCIPorts[i]);
            //设置各组合框的初始显示值
            if (SCIPorts.Length>0)
            {
                this.BtnSCISwitch.Enabled = true;
                this.CbSCIBaud.SelectedIndex = 0;
                this.CbSCIComNum.SelectedIndex = 0;
                this.CbSCISendType.SelectedIndex = 0;

                //设置初始的串口号与波特率
                PublicVar.g_SCIComNum = this.CbSCIComNum.Text;
                PublicVar.g_SCIBaudRate = int.Parse(this.CbSCIBaud.Text);
                //显示当前串口信与状态信息
                this.LblSCI.Text = str + PublicVar.g_SCIComNum + "、" +

                PublicVar.g_SCIBaudRate + "\n" + msg;
                this.TSSLState.Text = "无操作,请先选择波特率与串口号,打开串口," +
                                 "然后发送数据";
            }
            else
            {
                this.TSSLState.Text = "没有可用的串口,请检查!";
                this.BtnSCISwitch.Enabled = false;

            }
        }


        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:CbSCIBaud                                              
        /// 事    件:SelectedIndexChanged                                   
        /// 功    能:改变当前串口波特率                                     
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void CbSCIBaud_SelectedIndexChanged(object sender,
                                                    EventArgs e)
        {
            PublicVar.g_SCIBaudRate = int.Parse(this.CbSCIBaud.Text);
            this.TSSLState.Text = "过程提示:选择波特率";
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:CbSCIComNum:串口选择框                                 
        /// 事    件:SelectedIndexChanged                                  
        /// 功    能:改变当前串口号                                         
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void CbSCIComNum_SelectedIndexChanged(object sender,
                                                      EventArgs e)
        {
            PublicVar.g_SCIComNum = this.CbSCIComNum.Text;
            this.TSSLState.Text = "过程提示:选择串口号";
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:BtnSCISwitch                                           
        /// 事    件:Click                                                  
        /// 功    能:(1)当开关显示为打开串口(Open SCI),则单击时执行打开串口 
        ///          操作,并在标签LblSCI中显示选择的串口号与波特率,并在     
        ///          状态条文本TSSLState中显示当前操作                      
        ///          (2)当开关显示为关闭串口(Close SCI),则单击时执行关闭串口
        ///          操作,并在标签LblSCI中显示关闭的串口号与波特率,并在     
        ///          状态条文本TSSLState中显示当前操作                      
        /// 函数调用:(1)SCIInit:串口初始化                                  
        ///          (2)SCIClose:关闭串口                                   
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void BtnSCISwitch_Click(object sender, EventArgs e)
        {
            bool flag;//标记打开是否成功
            //根据按钮BtnSCISwitch显示内容执行打开或关闭串口操作
            if (this.BtnSCISwitch.Text.CompareTo("打开串口(Open SCI)") == 0)
            {
                //提示当前正在执行打开串口操作
                this.TSSLState.Text = "过程提示:正在打开串口...";
                //初始化SCI类对象sci
                sci = new SCI(PublicVar.g_SCIComNum,PublicVar.g_SCIBaudRate);
                if (sci.SCIOpen())//串口打开成功
                {
                    //设置接收中断处理事件
                    sci.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.SCIPort_DataReceived);
                    //设置每接收到1个字节中断1次
                    sci.SCIReceInt(1);

                    //显示打开串口相关信息
                    this.LblSCI.Text = str + PublicVar.g_SCIComNum + "、" + PublicVar.g_SCIBaudRate + "\n" + msg;
                    this.BtnSCISwitch.Text = "关闭串口(Close SCI)";//修改按钮上文字
                    this.CbSCIComNum.Enabled = false;//禁用[串口选择框]
                    this.CbSCIBaud.Enabled = false;  //禁用[波特率选择框]
                    //状态上显示结果信息
                    this.TSSLState.Text = this.TSSLState.Text +
                                          "打开" + PublicVar.g_SCIComNum + "成功!" + "波特率选择：" + PublicVar.g_SCIBaudRate;

                    this.BtnSCISend.Enabled = true;
                    this.button1.Enabled = true;


                }
                else//串口打开失败
                {
                    this.TSSLState.Text = this.TSSLState.Text +
                                          "打开" + PublicVar.g_SCIComNum + "失败!";

                    this.BtnSCISend.Enabled = false;
                }
            }
            else if (this.BtnSCISwitch.Text == "关闭串口(Close SCI)")
            {
                //设置接收中断处理事件
                sci.DataReceived -=new System.IO.Ports.SerialDataReceivedEventHandler(this.SCIPort_DataReceived);
                //提示当前操作
                this.TSSLState.Text = "过程提示:正在关闭串口...";
                //执行关闭串口操作,并用Flag返回结果
                flag = sci.SCIClose();
                if (flag == true)
                {
                    this.LblSCI.Text = str + PublicVar.g_SCIComNum
                             + "、" + PublicVar.g_SCIBaudRate + "\n" + msg;
                    this.BtnSCISwitch.Text = "打开串口(Open SCI)";
                    //[串口选择框]处于可用状态
                    this.CbSCIComNum.Enabled = true;
                    //[波特率选择框]处于可用状态
                    this.CbSCIBaud.Enabled = true;
                    this.TSSLState.Text += "关闭"+PublicVar.g_SCIComNum+"成功!";

                    this.BtnSCISend.Enabled = false;
                }
                else//串口关闭失败
                {
                    this.TSSLState.Text += "关闭"+PublicVar.g_SCIComNum+"失败!";
                }
            }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:BtnSCISend                                             
        /// 事    件:Click                                                  
        /// 功    能:发送数据:选择字符串发送时,以字符串的形式发送出去,      
        ///          选择十进制发送时,以十进制的形式发送                    
        ///          选择十六进制发送时,以十六进制的形式发送出              
        /// 函数调用:SCISendData:通过串口发送数据                           
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void BtnSCISend_Click(object sender, EventArgs e)
        {

            this.TSSLState.Text = "过程提示: 执行发送数据...";

            bool Flag;//判断数据发送是否成功
            int i, count = 0;//len保存发送数据的长度
            int len;

            //0表示选择是字符发送,1表示的是十进制发送,2表示十六进制发送
            int SendType;
            SendType = CbSCISendType.SelectedIndex;

            //定义一个ArrayList类的实例对象,实现一个数组,其大小在添加元
            //素时自动变化
            System.Collections.ArrayList SendData = new
                System.Collections.ArrayList();

            //如果串口没有打开
            if (!sci.IsOpen)
            {
                //状态条进行提示
                this.TSSLState.Text += "请先打开串口!";
                return;
            }
            //如果发送数据为空
            if (this.TbSCISend.Text == string.Empty)
            {
                this.TSSLState.Text += "发送数据不得为空!";
                return;
            }

            if (SendType == 0)//选择的是以字符串方式发送
            {
                this.TSSLState.Text = "以字符串方式发送数据!";
                //将要发送的数据进行编码,并获取编码后的数据长度
                len =System.Text.Encoding.Default.GetBytes(this.TbSCISend.Text).Length;
                //sci.SCIReceInt(SCIPort,len);//设置产生接收中断的字节数  【2014-5-5 注释，否则会导致程序无响应】
                //动态分配len字节单元内容用来存放发送数据

                //帧约定:帧头一字节(0x02),帧长数值两字节,有效数据n字节,帧尾一字节(0x03)
                PublicVar.g_SendByteArray = new byte[len+4];

                //获取TbSCISend文本的码值

                byte[] frameHead = new byte[] { 0x05 }; //帧头

                byte[] dataByteArray = System.Text.Encoding.Default.GetBytes(this.TbSCISend.Text); //有效数据

                byte[] dataLen = BitConverter.GetBytes(((UInt16)dataByteArray.Length)); //帧长

                byte[] frameTail = new byte[] { 0x03 }; //帧尾


                //将有效数据写入对应位置

                frameHead.CopyTo(PublicVar.g_SendByteArray, 0);
                dataLen.CopyTo(PublicVar.g_SendByteArray, 1);
                dataByteArray.CopyTo(PublicVar.g_SendByteArray, 2);
                frameTail.CopyTo(PublicVar.g_SendByteArray, 2 + len);



            }
            else //选择的是以十进制或者是十六进制发送数据
            {
                //sci.SCIReceInt(SCIPort, 1);//设置产生接收中断的字节数    【2014-5-5 注释，否则会导致程序无响应】
                foreach (string str in this.TbSCISend.Text.Split(','))
                {
                    //排除掉连续两个逗号,说明之间没有数
                    if (str != string.Empty)
                    {
                        if (SendType == 1)//选择的是以十进制方式发送
                        {
                            //将文本框中的数转化为十进制存入ArrayList类的实例对象
                            SendData.Add(Convert.ToByte(str, 10));
                            count++;//进行计数,统计有效的数据个数
                        }
                        else
                        {
                            //将文本框中的数转化为十六进制存入ArrayList类的实例对象
                            SendData.Add(Convert.ToByte(str, 16));
                            count++;//进行计数,统计有效的数据个数
                        }
                    }
                }
                //动态分配空间存放发送数据
                PublicVar.g_SendByteArray = new byte[count];

                //将已经转化后的数据放入到全局变量g_SendByteArray中
                for (i = 0; i < count; i++)
                    PublicVar.g_SendByteArray[i] = (byte)SendData[i];
            }

            //发送全局变量_SendByteArray中的数据,并返回结果
            Flag = sci.SCISendData(ref PublicVar.g_SendByteArray);

            if (Flag == true)//数据发送成功
                this.TSSLState.Text += "数据发送成功!";
            else
                this.TSSLState.Text += "数据发送失败!";
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:BtnSCIClearSend                                        
        /// 事    件:Click                                                  
        /// 功    能:清除发送框中的内容                                     
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void BtnSCIClearSend_Click(object sender, EventArgs e)
        {
            this.TbSCISend.Text = "";
            this.TSSLState.Text = "过程提示:清空发送文本框!";
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:SCIPort                                                
        /// 事    件:DataReceived                                           
        /// 功    能:串口接收数据                                           
        /// 函数调用:(1)SCIReceiveData,串口接收函数                         
        ///          (2)SCIUpdateRevtxtbox,更新文本框中的内容               
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void SCIPort_DataReceived(object sender,
            System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            String str = String.Empty;
            bool Flag;//标记串口接收数据是否成功
            int len;//标记接收的数据的长度

            byte[] ch2=new byte[2];
            //ComDevice.Encoding = System.Text.Encoding.GetEncoding("GB2312");




            //调用串口接收函数,并返回结果
            Flag = sci.SCIReceiveData(ref PublicVar.g_ReceiveByteArray);
                if (Flag == true)//串口接收数据成功
                {
                    len = PublicVar.g_ReceiveByteArray.Length;
                    //对于字符串形式,考虑到可能有汉字,
                    //直接调用系统定义的函数,处理整个字符串
                    str = Encoding.GetEncoding("GB2312").GetString(PublicVar.g_ReceiveByteArray);

                    SCIUpdateRevtxtbox(TbShowString, str);

                    //十进制和十六进制形式按字节进行处理
                    for (int i = 0; i < len; i++)
                    {
                        //十进制都是按照三位来显示,字节之间有空格表示区分
                        SCIUpdateRevtxtbox(TbShowDec,
                            PublicVar.g_ReceiveByteArray[i].ToString("D3") + "  ");
                        //十六进制都是按照两位来显示,字节之间有空格表示区分
                        SCIUpdateRevtxtbox(TbShowHex,
                            PublicVar.g_ReceiveByteArray[i].ToString("X2") + "  ");
                    }

                // 解析收到的数据
                parseRecivedData(PublicVar.g_ReceiveByteArray);
                   // sci.SCIReceInt(SCIPort, 1);//设置产生接收中断的字节数【2014-5-5 注释，否则会导致程序无响应】
                    this.TSSLState.Text = "过程提示:数据接收成功!";
                }
                //接收数据失败
                else
                {
                    //sci.SCIReceInt(SCIPort, 1);//设置产生接收中断的字节数【2014-5-5 注释，否则会导致程序无响应】 
                    this.TSSLState.Text = "过程提示:数据接收失败!";
                }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 函数名:SCIUpdateRevtxtbox                                       
        /// 参  数:(1)textbox,Object类型,接收数据要放入的文本框             
        ///        (2)text,string类型,要放入文本框的数据                    
        /// 功  能:若串行接收与Object不在同一线程中运行，那么通过invoke     
        ///        跨线程用串口接收到的数据来更新接收文本框中的数据         
        /// 返  回:无                                                       
        /// </summary>                                                      
        /// <param name="textbox"></param>                                  
        /// <param name="str"></param>                                      
        ///-----------------------------------------------------------------
        private void SCIUpdateRevtxtbox(Object textbox, string text)
        {
            //textbox显示文本与串口执行不在同一个线程中
            if (((TextBox)textbox).InvokeRequired)
            {
                handleinterfaceupdatedelegate InterFaceUpdate = new
                    handleinterfaceupdatedelegate(SCIUpdateRevtxtbox);
                this.Invoke(InterFaceUpdate, new object[] { textbox, text });
            }
            else
            {
                ((TextBox)textbox).Text += text;
                if (((TextBox)textbox).Text.Length >= 100)
                {
                    ((TextBox)textbox).Text = "";
                }
                    
                //把光标放在最后一行
                ((TextBox)textbox).SelectionStart =
                                           ((TextBox)textbox).Text.Length;
                //将文本框中的内容调整到当前插入符号位置
                ((TextBox)textbox).ScrollToCaret();
            }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:BtnSCIClearRec                                         
        /// 事    件:Click                                                  
        /// 功    能:清除接收文本框                                         
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void btnClearRec_Click(object sender, EventArgs e)
        {
            this.TbShowString.Text = string.Empty;
            this.TbShowDec.Text = string.Empty;
            this.TbShowHex.Text = string.Empty;
            this.TSSLState.Text = "过程提示:清空接收文本框!";
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:TbSCISend                                              
        /// 事    件:KeyPress                                                 
        /// 功    能:限制发送框内输入数据的格式                             
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///----------------------------------------------------------------- 
        private void TbSCISend_KeyPress(object sender, KeyPressEventArgs e)
        {
            int select = CbSCISendType.SelectedIndex;
            if (select == 1)//输入的是十进制的数
            {
                //除了数字、逗号和退格键,其他都不给输入
                if (e.KeyChar >= '0' && e.KeyChar <= '9' || e.KeyChar == 0x08
                    || e.KeyChar == ',')
                {
                    //输入的是数字,可以任意输入逗号与退格符
                    if (e.KeyChar >= '0' && e.KeyChar <= '9')
                    {
                        //在文本框中没有逗号时,
                        //this.TbSCISend.Text.LastIndexOf(',')默认为-1
                        //逗号之后出现第三个数字时,
                        //才用得着考虑是否会大于255
                        if (this.TbSCISend.Text.Length -
                            this.TbSCISend.Text.LastIndexOf(',') >= 2)
                        {
                            //考虑如果输入的话,是否会超出255
                            if (int.Parse(
                                    this.TbSCISend.Text.Substring(
                                 TbSCISend.Text.LastIndexOf(',') + 1)) * 10
                                    + e.KeyChar - '0' > 255)
                            {
                                e.Handled = true;
                                this.TSSLState.Text = "输入数据不得大于255";
                            }
                            //默认情况下是允许输入的,即e.Handled = false
                        }
                    }
                }
                else
                {
                    e.Handled = true;//除了逗号、数字0~9,其他都不给输入
                    this.TSSLState.Text = "输入数据必须是0-9,或者逗号"
                                          + ",或者退格符";
                }
            }
            //十六进制的处理方式与十进制相同,只是判断是否大于255时不太一样
            else if (select == 2)
            {
                //除了数字、大写字母、小写字母、逗号和退格键,其他都不给输入
                if (e.KeyChar >= '0' && e.KeyChar <= '9' || e.KeyChar >= 'a'
                    && e.KeyChar <= 'f' || e.KeyChar >= 'A' && e.KeyChar <=
                    'F' || e.KeyChar == 0x08 || e.KeyChar == ',')
                {
                    //逗号和退格符可以任意输入,只考虑输入数字的情况
                    if (e.KeyChar >= '0' && e.KeyChar <= '9')
                    {
                        if (this.TbSCISend.Text.Length -
                             this.TbSCISend.Text.LastIndexOf(',') >= 2)
                        {
                            if (Convert.ToInt32(TbSCISend.Text.Substring(
                               TbSCISend.Text.LastIndexOf(',') + 1), 16) * 16
                                + (e.KeyChar - '0') > 255)
                            {
                                e.Handled = true;
                                this.TSSLState.Text = "输入数据不得大于255";
                            }
                        }
                    }
                    else if (e.KeyChar >= 'a' && e.KeyChar <= 'f'
                        || e.KeyChar >= 'A' && e.KeyChar <= 'F')
                    {
                        if (this.TbSCISend.Text.Length -
                             this.TbSCISend.Text.LastIndexOf(',') >= 2)
                        {
                            //无论是大写字母还是小写字母都转化成大写字母判断
                            if (Convert.ToInt32(TbSCISend.Text.Substring(
                               TbSCISend.Text.LastIndexOf(',') + 1), 16) * 16
                                + (Char.ToUpper(e.KeyChar) - 'A') > 255)
                            {
                                e.Handled = true;
                                this.TSSLState.Text = "输入数据不得大于255";
                            }
                        }
                    }
                }
                else
                {
                    e.Handled = true;
                    this.TSSLState.Text = "输入数据必须是0-9,a-f,A-F,或者逗号"
                                          + ",或者退格符";
                }
            }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// /// 对    象:CbSCISendType                                          
        /// 事    件:SelectedIndexChanged                                   
        /// 功    能:当选泽发送方式时,提示用户输入数据的格式,并清空发送框   
        ///         (1)当选中"字符串方式(String)"时,提示用户"请输入字符串"  
        ///         (2)当选中"十进制方式(Decimal)"时,提示用户"请输入十进制数
        ///            ,以逗号隔开,数据范围0-255,允许用退格键"              
        ///         (3)当选中"十六进制方式(Hex):,提示用户"请输入十六进制数, 
        ///            以逗号隔开,数据范围00-FF,允许用退格键"               
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void CbSCISendType_SelectedIndexChanged(object sender,
                                                        EventArgs e)
        {
            this.TSSLState.Text = "过程提示:选择数据发送方式!";
            //选择某一种发送方式时,首先清空发送框
            this.TbSCISend.Text = string.Empty;

            //选择的是发送字符串
            if (CbSCISendType.SelectedIndex == 0)
            {
                this.LabNote.Text = "请输入字符串!";
                this.TSSLState.Text = "你选择了发送字符串!";
            }
            else if (CbSCISendType.SelectedIndex == 1)
            {
                this.LabNote.Text = "请输入十进制数,以逗号(英文输入法下)隔开"
                                    + ",数据范围0-255,允许用退格键";
                this.TSSLState.Text = "你选择了发送十进制数!";
            }
            else
            {
                this.LabNote.Text = "请输入十六进制数,以逗号(英文输入法下)隔"
                                    + "开,数据范围00-FF,允许用退格键";
                this.TSSLState.Text = "你选择了发送十六进制数! ";
            }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:BtnState                                               
        /// 事    件:Click                                                  
        /// 功    能:控制状态条显示或隐藏                                   
        /// 函数调用:无                                                     
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void BtnState_Click(object sender, EventArgs e)
        {
            //状态条是不可见的
            if (this.TSSLState.Visible == false)
            {
                this.sSSerialPortInfo.Visible = true;//状态条可见
                BtnState.Text = "隐藏状态条(Hide)";
            }
            //当前状态条不可见
            else
            {
                this.sSSerialPortInfo.Visible = false;//状态条不可见
                BtnState.Text = "显示状态条(Show)";
            }
        }

        ///-----------------------------------------------------------------
        /// <summary>                                                       
        /// 对    象:FrmSCI                                                 
        /// 事    件:FormClosing                                               
        /// 功    能:关闭窗体时,确保串口已经关闭                            
        /// 函数调用:SCIClose                                               
        /// </summary>                                                      
        /// <param name="sender"></param>                                   
        /// <param name="e"></param>                                        
        ///-----------------------------------------------------------------
        private void FrmSCI_FormClosing(object sender,
            FormClosingEventArgs e)
        {
            try
            {
                sci.SCIClose();
            }
            catch
            { }
        }


        //------------------------------------------------------------------

        // Send data through uart
        private void sendUARTData(string data, object sender, EventArgs e)
        {
            
            BtnSCIClearSend_Click(sender, e);
            this.TbSCISend.Text = data;
            BtnSCISend_Click(sender, e);
            Delay(100);
        }
        
        // 发送请求节点信息数据 uart
        private void sendQueryActiveNodes(object sender, EventArgs e)
        {
            sendUARTData("n", sender, e);
        }

        // 发生请求节点温度数据 uart nodestr: "1" or "2" ...
        private void sendQueryTempInfo(string nodeStr, object sender, EventArgs e)
        {
            string dataToSend = "t" + nodeStr;
            sendUARTData(dataToSend, sender, e);
        }

        //对某个节点发送持续监测信号
        private void sendContinusMinotor(string nodeStr, object sender, EventArgs e) 
        {
            string dataToSend = "c" + nodeStr;
            sendUARTData(dataToSend, sender, e);
        }
        //对某个节点发送读取数据信号
        private void sendDataRead(byte nodeaddr, object sender, EventArgs e)
        {
            byte[] dataToSend = { (byte)'r',nodeaddr };
            string str = System.Text.Encoding.Default.GetString(dataToSend);
            sendUARTData(str, sender, e);
        }
        //对某个节点发送丢失帧信息
        private void sendPCMissFrames(byte nodeaddr, byte[] missDatas, object sender, EventArgs e)
        {
            Delay(500);
            byte[] dataToSend = new byte[missDatas[0] + 3];
            dataToSend[0] = (byte)'M';
            dataToSend[1] = nodeaddr;
            Array.Copy(missDatas, 0, dataToSend, 2, missDatas[0] + 1);
            string str = System.Text.Encoding.Default.GetString(dataToSend);
            sendUARTData(str, sender, e);
        }

        // 发送灯控制信息
        public void sendLightControlMessage(byte nodeaddr, byte value, object sender, EventArgs e)
        {
            byte[] dataToSend = new byte[3];
            dataToSend[0] = (byte)'L';
            dataToSend[1] = nodeaddr;
            dataToSend[2] = value;
            string str = System.Text.Encoding.Default.GetString(dataToSend);
            sendUARTData(str, sender, e);
        }


        // 发送大数据，头
        // totalLength 共多少帧数据
        private void sendBigDataStart(byte nodeAddr, byte totalLength, object sender, EventArgs e) {
            byte[] lengthAndAddr = {totalLength, nodeAddr};
            string str = System.Text.Encoding.Default.GetString(lengthAndAddr);
            string dataToSend = "l" + str;
            sendUARTData(dataToSend, sender, e);
        }

        // 发送大数据，数据帧
        private void sendBigDataFrame(byte frameOrder,byte[] data, object sender, EventArgs e) {
            byte len = (byte)data.Length;
            byte[] lenArr = {len};
            byte[] orderArr = { frameOrder };
            string dataStr = System.Text.Encoding.Default.GetString(data);
            string len_str = System.Text.Encoding.Default.GetString(lenArr);
            string frameOrder_str = Encoding.Default.GetString(orderArr);
            string dataToSend = "b"+frameOrder_str + len_str + dataStr;
            sendUARTData(dataToSend, sender, e);
        }

        // 发送大数据，尾
        private void sendBigDataEnd(object sender, EventArgs e) {
            sendUARTData("e", sender, e);
        }

        // 获取 subarray
        public static byte[] SubArray(byte[] data, int index, int length)
        {
            byte[] result = new byte[length];
            try
            {
                Array.Copy(data, index, result, 0, length);
            }
            catch
            { Console.WriteLine("Error the get sub array");      }
            return result;
        }


        ///-----------------------------------------------------------------
        /// <summary>                                                                                         
        /// 功    能:向某个节点发送大数据                                                                   
        /// </summary>                                                      
        /// <param name="nodeAddr"></param>                                   
        /// <param name="data"></param>    
        ///  <param name="sender"></param>    
        ///  <param name="e"></param>    
        ///-----------------------------------------------------------------
        private void sendBigData(byte nodeAddr, byte[] data, object sender, EventArgs e) {

            //计算发送次数
            int FrameLength = MaxFrameLength;
            int counts = data.Length / FrameLength;
            int lastFrameLength = data.Length % FrameLength;
            int sendCount = counts;
            if (lastFrameLength != 0) {
                sendCount += 1;
            }
            //开启进度条
            pgb = new _02_Form.ProcessBarForm();
            pgb.Show();

            //发送开始传输消息
            sendBigDataStart(nodeAddr, (byte)sendCount, sender, e);

           //发送每一帧
            for (int i = 0; i < counts; i++) {
                Console.Write("send frame :");
                Console.WriteLine(i.ToString());
                byte[] toSend = SubArray(data, i*FrameLength, FrameLength);
                sendBigDataFrame((byte)i,toSend, sender, e);
                textBox3.Text = string.Format("正在发送第{0}帧...\r\n", i+1) + textBox3.Text;
                string t = string.Format("{0}/{1}", i+1, sendCount);
                pgb.updateText(t);
                pgb.updateValue((i + 1) * 100 / sendCount);
            }
            //发送最后一帧
            if (lastFrameLength != 0) {
                byte[] toSend = SubArray(data, counts*FrameLength, lastFrameLength);
                sendBigDataFrame((byte)(sendCount-1),toSend, sender, e);
                Console.Write("send frame :");
                Console.WriteLine(counts.ToString());
                textBox3.Text = string.Format("正在发送第{0}帧...\r\n", sendCount) + textBox3.Text;
                string t = string.Format("{0}/{1}", sendCount, sendCount);
                pgb.updateText(t);
                pgb.updateValue(100);
            }
            //发送结束信息
            sendBigDataEnd(sender, e);
            Console.Write("send end.");
            this.mTimeoutObject = new ManualResetEvent(true);
            pgb.Close();
        }



        // node 注册成功通知
        private void nodeHaveRegistered(byte nodeAddr)
        {
            nodeAddr =(byte)( (int)nodeAddr -1);
            new Animator2D(
                new Path2D(MyBarWithPic[nodeAddr].pic.Location.X, 150, MyBarWithPic[nodeAddr].pic.Location.Y, MyBarWithPic[nodeAddr].pic.Location.Y, 1000)
                )
            .Play(MyBarWithPic[nodeAddr].pic, Animator2D.KnownProperties.Location);

            new Animator2D(
                new Path2D(MyBarWithPic[nodeAddr].bar.Location.X, 5, MyBarWithPic[nodeAddr].bar.Location.Y, MyBarWithPic[nodeAddr].bar.Location.Y, 1000)
             )
            .Play(MyBarWithPic[nodeAddr].bar, Animator2D.KnownProperties.Location);

        }

        // node 离开消息
        private void nodeHaveLeft(byte nodeAddr)
        {
            nodeAddr = (byte)((int)nodeAddr - 1);
            new Animator2D(
                new Path2D(MyBarWithPic[nodeAddr].pic.Location.X, 300, MyBarWithPic[nodeAddr].pic.Location.Y, MyBarWithPic[nodeAddr].pic.Location.Y, 1000)
                )
            .Play(MyBarWithPic[nodeAddr].pic, Animator2D.KnownProperties.Location);

            new Animator2D(
                new Path2D(MyBarWithPic[nodeAddr].bar.Location.X,404 , MyBarWithPic[nodeAddr].bar.Location.Y, MyBarWithPic[nodeAddr].bar.Location.Y, 1000)
             )
            .Play(MyBarWithPic[nodeAddr].bar, Animator2D.KnownProperties.Location);

        }





        // nodes 在线的通知
        private void nodesHaveChanged(int nodesNum, byte[] nodesAddrs,bool isIn)
        {
    
            for(int j = 0; j < 6; j++)
            {
                bool isFind = false;
                for (int i = 0; i < nodesAddrs.Length; i++)
                {
                    if((j+1)== nodesAddrs[i])
                    {
                        isFind = true;
                        if (isIn)
                        {
                            nodeHaveRegistered(nodesAddrs[i]);

                            String info = String.Format("\n新节点在网络注册,地址:  {0:G}\r\n", nodesAddrs[i]);
                            this.textBox3.Text = info + this.textBox3.Text;
                            this.textBox3.ScrollToCaret();
                           
                        }
                        else
                        {
                            nodeHaveLeft(nodesAddrs[i]);

                            String info = String.Format("\n节点离开网络,地址:  {0:G}\r\n", nodesAddrs[i]);
                            this.textBox3.Text = info+ this.textBox3.Text;
                            this.textBox3.ScrollToCaret();
                        }
                        break;
                    }
                }
                if (!isFind)
                {
                    
                    if(isIn)
                    {
                        nodeHaveLeft((byte)(j+1));
                    }
                    
                }
            }

        }

        // 收到 node 发送的温度通知
        private void nodeHaveGottenTemp(byte nodeAddr, float temp)
        {
            String info = String.Format("节点[{0:G}]当前光照强度:", nodeAddr) + temp.ToString() + "\r\n";
            this.textBox3.Text = info + this.textBox3.Text;

            nodeAddr = (byte)((int)nodeAddr - 1);

            if (temp - 130 > 100)
            {
                temp = 130 + 100;
            }
            if(temp - 130 < 0)
            {
                temp = 130 + 0;
            }
            new Animator(
                new WinFormAnimation.Path(MyBarWithPic[nodeAddr].bar.Value, Math.Abs(temp - 130), 300)
                ).Play(MyBarWithPic[nodeAddr].bar, Animator.KnownProperties.Value);
            
        }

        //pc与pc_node 通信协议标志
        enum FFDDataType
        {
            RegisterSuccess = 's',  // s|node address
            NodeStatus = 'n',       // n|num of nodes|node0|node1...
            TempInfo = 't',       // t|node address|temp(float)
            ADCContinuousMonitor = 'c', // c
            NodeDeathInfo = 'd',  //d|death of node
            BigDataStart  = 'l',   // l | totoalLength | destination
            BigData       = 'D',   // D | dataLength | data
            BigDataEnd    = 'e'  ,  // e 
            BigDataMiss = 'm', // m | miss number | miss frame orders
        }

        // 解析收到的信息
        private void parseRecivedData(byte[] receiveData)
        {

            if (receiveData.Length <= 0)
            {
                return;
            }
            switch (receiveData[0])
            {
                //注册成功消息
                case (byte)FFDDataType.RegisterSuccess:
                    nodeHaveRegistered(receiveData[1]);
                    String info = String.Format("\n新节点在网络注册,地址:  {0:G}\r\n", receiveData[1]);
                    this.textBox3.Text = info + this.textBox3.Text;
                    break;
                //节点状态消息
                case (byte)FFDDataType.NodeStatus:
                    byte nodesNum = receiveData[1];
                    byte[] nodesAddrs = new byte[nodesNum];
                    for (int i = 0; i < nodesNum; i = i + 1)
                    {
                        nodesAddrs[i] = receiveData[i + 2];
                    }
                    if (nodesNum == 0)
                    {
                        this.textBox3.Text = "当前网络中无节点\r\n" + this.textBox3.Text;
                    }
                    nodesHaveChanged(nodesNum, nodesAddrs,true);
                    break;
                //节点离开信息
                case (byte)FFDDataType.NodeDeathInfo:
                    byte[] addr = new byte[1];
                    addr[0] = receiveData[1];
                    nodesHaveChanged(1, addr,false);
                    break;
                //收到节点物理测量消息
                case (byte)FFDDataType.TempInfo:
                    float temp = System.BitConverter.ToSingle(receiveData,2);
                    nodeHaveGottenTemp(receiveData[1], temp);
                    break;
                //节点传输开始消息
                case (byte)FFDDataType.BigDataStart:
                    backgroundWorker1.RunWorkerAsync();
                    recv_count = 0;
                    pgb = new _02_Form.ProcessBarForm();
                    //pgb_.Show();
                    //保证线程安全
                    if (InvokeRequired)
                    {
                        this.Invoke(new Action(() => pgb.Show())); //显示一个进度条
                    }
                    //根据发送进度更新进度条
                    pgb.updateValue(0);
                    img_byte_list = new List<byte>();
                    img_to_show = new byte[MaxFrameLength * receiveData[2]];
                    totalLength = (int)receiveData[2];
                    recv_data_flag = new byte[receiveData[2]];
                    not_recv_datas = new byte[receiveData[2]];
                    big_data_source_addr = receiveData[1];
                    break;
                //收到节点的某一帧
                case (byte)FFDDataType.BigData:

                    int frameOrder = receiveData[1];
                    recv_count++;
                    string t = string.Format("{0}/{1}", recv_count, totalLength);

                    //保证线程安全
                    if (InvokeRequired)
                    {
                        //根据接收进度更新进度条
                        this.Invoke(new Action(() => pgb.updateValue((recv_count)*100/totalLength)));
                        this.Invoke(new Action(() => pgb.updateText(t)));
                    }
                    //更新log消息
                    textBox3.Text = string.Format("正在接收第{0}帧...\r\n", frameOrder)+ textBox3.Text;
                    Console.Write("rcev  data frame:");
                    Console.Write(frameOrder.ToString()+"/");
                    Console.WriteLine(totalLength.ToString());
                    recv_data_flag[frameOrder] = 1;
                    Array.Copy(receiveData, 2, img_to_show, frameOrder * MaxFrameLength, receiveData.Length - 2);
                    break;
                //节点传输数据结束
                case (byte)FFDDataType.BigDataEnd:
                    int not_recv_count = 0;
                    for (int i = 0; i < recv_data_flag.Length; i++)
                    {
                        if (recv_data_flag[i] == 0)
                        {
                            not_recv_count++;
                            not_recv_datas[not_recv_count] = (byte)i;
                        }
                    }
                    not_recv_datas[0] = (byte)not_recv_count;
                    Console.Write("pc read: miss ");
                    Console.Write(not_recv_count.ToString());
                    Console.WriteLine(" frames");
                    //发送丢失的帧
                    sendPCMissFrames(big_data_source_addr, not_recv_datas, null, null);
                    if (not_recv_count == 0)
                    {
                        showAvatorImage(img_to_show, big_data_source_addr);
                        MessageBox.Show("传输完成");
                    }
                    else
                    {
                        textBox3.Text = string.Format("共缺少{0}帧,正在重传...\r\n", not_recv_count) + textBox3.Text;
                    }
               
                    break;
                //来自节点的丢失帧信息
                case (byte)FFDDataType.BigDataMiss:
                    int missNumber = receiveData[1];
                    textBox3.Text = string.Format("共缺少{0}帧,正在重传...\r\n", missNumber) + textBox3.Text;
                    Console.Write("miss frame:");
                    Console.WriteLine(missNumber.ToString());
                    byte[] missOrder = new byte[missNumber];
                    SubArray(receiveData, 2, missNumber).CopyTo(missOrder,0);
                    //重传每个丢失帧
                    foreach(byte order in missOrder)
                    {

                        byte[] toSend = SubArray(buff, order * MaxFrameLength, MaxFrameLength);
                        sendBigDataFrame(order, toSend, null, null);
                        textBox3.Text = string.Format("正在发送第{0}帧...\r\n", order) + textBox3.Text;
                    }
                    sendBigDataEnd(null, null);
                    break;
                default:
                    break;
            }
        }

        private void showAvatorImage(byte[] buff_, int avatorNum)
        {
            Image res_img = convertImg(buff);
            switch (avatorNum)
            {
                case 1:
                    pictureBox1.Image = res_img;
                    break;
                case 2:
                    pictureBox2.Image = res_img;
                    break;
                case 3:
                    pictureBox3.Image = res_img;
                    break;
            }
            
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.textBox3.Text = "";
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left )
            {
                sendQueryTempInfo("1", sender, e);
            }
        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left)
            {
                sendQueryTempInfo("2", sender, e);
            }
        }

        private void pictureBox3_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left)
            {
                sendQueryTempInfo("3", sender, e);
            }
        }

        private void pictureBox4_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left)
            {
                sendQueryTempInfo("4", sender, e);
            }
        }

        private void pictureBox5_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left)
            {
                sendQueryTempInfo("5", sender, e);
            }
        }

        private void pictureBox6_Click(object sender, EventArgs e)
        {
            MouseEventArgs Mouse_e = (MouseEventArgs)e;
            if (Mouse_e.Button == MouseButtons.Left)
            {
                sendQueryTempInfo("6", sender, e);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            sendQueryActiveNodes(sender, e);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
        private void updateProgressValue(int percent)
        {

        }

        private void pictureBox1_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 0;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr + 1)+this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
                
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }

            sendContinusMinotor("1", sender, e);

        }


        public static void Delay(int milliSecond)
        {
            int start = Environment.TickCount;
            while (Math.Abs(Environment.TickCount - start) < milliSecond)
            {
                Application.DoEvents();
            }
        }

        private void pictureBox2_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 1;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }
            sendContinusMinotor("2", sender, e);

        }

        private void pictureBox3_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 2;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr+1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
        
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }

            sendContinusMinotor("3", sender, e);
        }

        private void pictureBox4_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 3;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
               
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }
            sendContinusMinotor("4", sender, e);

        }

        private void pictureBox5_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 4;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
                
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }
            sendContinusMinotor("5", sender, e);

        }

        private void pictureBox6_DoubleClick(object sender, EventArgs e)
        {
            int nodeaddr = 5;
            if (MyBarWithPic[nodeaddr].isMonitored)
            {
                this.textBox3.Text = String.Format("停止监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = false;
                
            }
            else
            {
                this.textBox3.Text = String.Format("开始持续监测节点[{0:G}]\r\n", nodeaddr + 1) + this.textBox3.Text;
                MyBarWithPic[nodeaddr].isMonitored = true;
                
            }
            sendContinusMinotor("6", sender, e);

        }

        private byte[] convertByte(Image img)
        {
            MemoryStream ms = new MemoryStream();
            img.Save(ms, img.RawFormat);
            //byte[] bytes = new byte[ms.Length];
            //ms.Read(bytes, 0, Convert.ToInt32(ms.Length));
            //以上两句改成下面两句
            byte[] bytes = ms.ToArray();
            ms.Close();
            return bytes;
        }

        ///二进制生成图片
        private Image convertImg(byte[] datas)
        {
            MemoryStream ms = new MemoryStream(datas);
            Image img = Image.FromStream(ms, true);
            ms.Close();
            return img;
        }

        private void 发送文件ToolStripMenuItem_Click(object sender, EventArgs e)
        {


            string a= ((ContextMenuStrip)((ControlAccessibleObject)((System.Windows.Forms.ToolStripMenuItem)sender).AccessibilityObject.Parent).Owner).SourceControl.Name.ToString();
            int addr = a[a.Length - 1] - '0';
            OpenFileDialog fileDialog = new OpenFileDialog();
            fileDialog.Multiselect = false;
            fileDialog.Title = "请选择文件";
            fileDialog.Filter = "Image Files(*.BMP;*.JPG;*.GIF)|*.BMP;*.JPG;*.GIF";
            if (fileDialog.ShowDialog() == DialogResult.OK)
            {

                string filename = fileDialog.FileNames[0];

                MessageBox.Show("已选择文件:" + filename, "选择文件提示", MessageBoxButtons.OK, MessageBoxIcon.Information);
                FileStream file = new FileStream(filename, FileMode.Open);
                file.Seek(0, SeekOrigin.Begin);


                BinaryReader br = new BinaryReader(file);
                long numBytes = new FileInfo(filename).Length;
                buff = br.ReadBytes((int)numBytes);


                sendBigData((byte)addr, buff, sender, e);


                file.Close();

            }
        }

        private void 查看图片ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string a = ((ContextMenuStrip)((ControlAccessibleObject)((System.Windows.Forms.ToolStripMenuItem)sender).AccessibilityObject.Parent).Owner).SourceControl.Name.ToString();
            int addr = a[a.Length - 1] - '0';
            sendDataRead((byte)addr, sender, e);
        }

        private void 控制小灯ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string a = ((ContextMenuStrip)((ControlAccessibleObject)((System.Windows.Forms.ToolStripMenuItem)sender).AccessibilityObject.Parent).Owner).SourceControl.Name.ToString();
            int addr = a[a.Length - 1] - '0';
            var lightFrequentForm = new _02_Form.小灯闪烁频率控制((byte)addr,this);
            lightFrequentForm.Show();

        }

        private void 帮助ToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void 帮助ToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            
        }
    }



}