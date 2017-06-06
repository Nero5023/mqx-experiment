using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WinFormAnimation;

namespace SerialPort._02_Form
{

    public partial class ProcessBarForm : Form
    {
        private int totalNumber;
        public ProcessBarForm()
        {
            InitializeComponent();

        }
        public void updateValue(int value)
        {
            this.pb.Value = value;
        }
        public void updateText(string t)
        {
            this.value_label.Text = t;
            
        }

    }
}
