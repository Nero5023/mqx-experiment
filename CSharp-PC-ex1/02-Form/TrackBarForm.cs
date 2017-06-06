using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SerialPort._02_Form
{
    public partial class 小灯闪烁频率控制 : Form
    {
        private byte addr;
        private FrmSCI f;
        public 小灯闪烁频率控制(byte a,FrmSCI f)
        {
            InitializeComponent();
            this.addr = a;
            this.f = f;
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            this.f.sendLightControlMessage(this.addr, (byte)this.trackBar1.Value, sender, e);
        }
    }
}
