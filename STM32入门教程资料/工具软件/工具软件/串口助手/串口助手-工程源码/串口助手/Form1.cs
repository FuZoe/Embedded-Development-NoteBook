using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Windows.Forms;

namespace 串口助手
{
    public partial class Form1 : Form
    {

        string receiveMode = "HEX模式";
        string receiveCoding = "GBK";
        string sendMode = "HEX模式";
        string sendCoding = "GBK";

        List<byte> byteBuffer = new List<byte>();       //接收字节缓存区

        private string BytesToText(byte[] bytes, string encoding)       //字节流转文本
        {
            List<byte> byteDecode = new List<byte>();   //需要转码的缓存区
            byteBuffer.AddRange(bytes);     //接收字节流到接收字节缓存区
            if (encoding == "GBK")
            {
                int count = byteBuffer.Count;
                for (int i = 0; i < count; i ++)
                {
                    if (byteBuffer.Count == 0)
                    {
                        break;
                    }
                    if (byteBuffer[0] < 0x80)       //1字节字符
                    {
                        byteDecode.Add(byteBuffer[0]);
                        byteBuffer.RemoveAt(0);
                    }
                    else       //2字节字符
                    {
                        if (byteBuffer.Count >= 2)
                        {
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                        }
                    }
                }
            }
            else if (encoding == "UTF-8")
            {
                int count = byteBuffer.Count;
                for (int i = 0; i < count; i++)
                {
                    if (byteBuffer.Count == 0)
                    {
                        break;
                    }
                    if ((byteBuffer[0] & 0x80) == 0x00)     //1字节字符
                    {
                        byteDecode.Add(byteBuffer[0]);
                        byteBuffer.RemoveAt(0);
                    }
                    else if ((byteBuffer[0] & 0xE0) == 0xC0)     //2字节字符
                    {
                        if (byteBuffer.Count >= 2)
                        {
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                        }
                    }
                    else if ((byteBuffer[0] & 0xF0) == 0xE0)     //3字节字符
                    {
                        if (byteBuffer.Count >= 3)
                        {
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                        }
                    }
                    else if ((byteBuffer[0] & 0xF8) == 0xF0)     //4字节字符
                    {
                        if (byteBuffer.Count >= 4)
                        {
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                            byteDecode.Add(byteBuffer[0]);
                            byteBuffer.RemoveAt(0);
                        }
                    }
                    else        //其他
                    {
                        byteDecode.Add(byteBuffer[0]);
                        byteBuffer.RemoveAt(0);
                    }
                }
            }
            return Encoding.GetEncoding(encoding).GetString(byteDecode.ToArray());
        }

        private string BytesToHex(byte[] bytes)     //字节流转HEX
        {
            string hex = "";
            foreach (byte b in bytes)
            {
                hex += b.ToString("X2") + " ";
            }
            return hex;
        }

        private byte[] TextToBytes(string str, string encoding)     //文本转字节流
        {
            return Encoding.GetEncoding(encoding).GetBytes(str);
        }

        private byte[] HexToBytes(string str)       //HEX转字节流
        {
            string str1 = Regex.Replace(str, "[^A-F^a-f^0-9]", "");     //清除非法字符

            double i = str1.Length;     //将字符两两拆分
            int len = 2;
            string[] strList = new string[int.Parse(Math.Ceiling(i / len).ToString())];
            for (int j = 0; j < strList.Length; j++)
            {
                len = len <= str1.Length ? len : str1.Length;
                strList[j] = str1.Substring(0, len);
                str1 = str1.Substring(len, str1.Length - len);
            }

            int count = strList.Length;     //将拆分后的字符依次转换为字节
            byte[] bytes = new byte[count];
            for (int j = 0; j < count; j ++)
            {
                bytes[j] = byte.Parse(strList[j], NumberStyles.HexNumber);
            }

            return bytes;
        }

        private void OpenSerialPort()       //打开串口
        {
            try
            {
                serialPort.PortName = cbPortName.Text;
                serialPort.BaudRate = Convert.ToInt32(cbBaudRate.Text);
                serialPort.DataBits = Convert.ToInt32(cbDataBits.Text);
                StopBits[] sb = { StopBits.One, StopBits.OnePointFive, StopBits.Two };
                serialPort.StopBits = sb[cbStopBits.SelectedIndex];
                Parity[] pt = { Parity.None, Parity.Odd, Parity.Even };
                serialPort.Parity = pt[cbParity.SelectedIndex];
                serialPort.Open();

                btnOpen.BackColor = Color.Pink;
                btnOpen.Text = "关闭串口";
                btnSend.Enabled = true;
                cbPortName.Enabled = false;
                cbBaudRate.Enabled = false;
                cbDataBits.Enabled = false;
                cbStopBits.Enabled = false;
                cbParity.Enabled = false;

            }
            catch
            {
                MessageBox.Show("串口打开失败", "提示");
            }
        }

        private void CloseSerialPort()      //关闭串口
        {
            serialPort.Close();

            btnOpen.BackColor = SystemColors.ControlLight;
            btnOpen.Text = "打开串口";
            btnSend.Enabled = false;
            cbPortName.Enabled = true;
            cbBaudRate.Enabled = true;
            cbDataBits.Enabled = true;
            cbStopBits.Enabled = true;
            cbParity.Enabled = true;
        }

        public Form1()
        {
            InitializeComponent();
        }
        
        private void Form1_Load(object sender, EventArgs e)        //窗口加载事件
        {
            cbBaudRate.SelectedIndex = 1;       //控件状态初始化
            cbDataBits.SelectedIndex = 3;
            cbStopBits.SelectedIndex = 0;
            cbParity.SelectedIndex = 0;
            cbReceiveMode.SelectedIndex = 0;
            cbReceiveCoding.SelectedIndex = 0;
            cbSendMode.SelectedIndex = 0;
            cbSendCoding.SelectedIndex = 0;
            btnSend.Enabled = false;
            cbPortName.Enabled = true;
            cbBaudRate.Enabled = true;
            cbDataBits.Enabled = true;
            cbStopBits.Enabled = true;
            cbParity.Enabled = true;
        }

        private void cbPortName_DropDown(object sender, EventArgs e)        //串口号下拉事件
        {
            string currentName = cbPortName.Text;
            string[] names = SerialPort.GetPortNames();       //搜索可用串口号并添加到下拉列表
            cbPortName.Items.Clear();
            cbPortName.Items.AddRange(names);
            cbPortName.Text = currentName;
        }

        private void btnOpen_Click(object sender, EventArgs e)      //打开串口点击事件
        {
            if (btnOpen.Text == "打开串口")
            {
                OpenSerialPort();
            }
            else if (btnOpen.Text == "关闭串口")
            {
                CloseSerialPort();
            }

        }

        protected override void DefWndProc(ref Message m)       //USB拔出事件
        {
            
            if (m.Msg == 0x0219)        //WM_DEVICECHANGE
            {
                if (m.WParam.ToInt32() == 0x8004)
                {
                    if (btnOpen.Text == "关闭串口" && serialPort.IsOpen == false)
                    {
                        CloseSerialPort();      //USB异常拔出，关闭串口
                    }
                }
            }
            base.DefWndProc(ref m);
        }

        private void btnSend_Click(object sender, EventArgs e)      //发送点击事件
        {
            if (serialPort.IsOpen)
            {
                if (sendMode == "HEX模式")
                {
                    byte[] dataSend = HexToBytes(tbSend.Text);      //HEX转字节流
                    int count = dataSend.Length;
                    serialPort.Write(dataSend, 0, count);       //串口发送
                }
                else if (sendMode == "文本模式")
                {
                    byte[] dataSend = TextToBytes(tbSend.Text, sendCoding);      //文本转字节流
                    int count = dataSend.Length;
                    serialPort.Write(dataSend, 0, count);       //串口发送
                }
            }
        }

        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)      //串口接收数据事件
        {
            if (serialPort.IsOpen)
            {
                int count = serialPort.BytesToRead;
                byte[] dataReceive = new byte[count];
                serialPort.Read(dataReceive, 0, count);     //串口接收

                this.BeginInvoke((EventHandler)(delegate
                {
                    if (receiveMode == "HEX模式")
                    {
                        tbReceive.AppendText(BytesToHex(dataReceive));  //字节流转HEX
                    }
                    else if (receiveMode == "文本模式")
                    {
                        tbReceive.AppendText(BytesToText(dataReceive, receiveCoding));       //字节流转文本
                    }
                }));
            }
        }
        
        private void btnClearReceive_Click(object sender, EventArgs e)      //清空接收区点击事件
        {
            tbReceive.Clear();
        }

        private void btnClearSend_Click(object sender, EventArgs e)      //清空发送区点击事件
        {
            tbSend.Clear();
        }

        private void cbReceiveMode_SelectedIndexChanged(object sender, EventArgs e)     //接收模式选择事件
        {
            if (cbReceiveMode.Text == "HEX模式")
            {
                cbReceiveCoding.Enabled = false;
                receiveMode = "HEX模式";
            }
            else if (cbReceiveMode.Text == "文本模式")
            {
                cbReceiveCoding.Enabled = true;
                receiveMode = "文本模式";
            }
            byteBuffer.Clear();
        }

        private void cbReceiveCoding_SelectedIndexChanged(object sender, EventArgs e)     //接收编码选择事件
        {
            if (cbReceiveCoding.Text == "GBK")
            {
                receiveCoding = "GBK";
            }
            else if (cbReceiveCoding.Text == "UTF-8")
            {
                receiveCoding = "UTF-8";
            }
            byteBuffer.Clear();
        }

        private void cbSendMode_SelectedIndexChanged(object sender, EventArgs e)     //发送模式选择事件
        {
            if (cbSendMode.Text == "HEX模式")
            {
                cbSendCoding.Enabled = false;
                sendMode = "HEX模式";
            }
            else if (cbSendMode.Text == "文本模式")
            {
                cbSendCoding.Enabled = true;
                sendMode = "文本模式";
            }
        }

        private void cbSendCoding_SelectedIndexChanged(object sender, EventArgs e)     //发送编码选择事件
        {
            if (cbSendCoding.Text == "GBK")
            {
                sendCoding = "GBK";
            }
            else if (cbSendCoding.Text == "UTF-8")
            {
                sendCoding = "UTF-8";
            }
        }
    }
}

