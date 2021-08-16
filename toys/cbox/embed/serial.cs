using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace proto {
    public class serial {
        public const string COM_PORT = "COM13"; // ADP
        //public const string COM_PORT = "COM32"; // MCPA
        //public const string COM_PORT = "COM33"; // MCPB
        //public const string COM_PORT = "COM34"; // MCPC
        public serial() {
            port_name = COM_PORT;
            serialPort = new SerialPort();
            serialPort.BaudRate = 115200;
            serialPort.DataBits = 8;
            serialPort.Parity = Parity.None;
            serialPort.StopBits = StopBits.One;
            serialPort.DataReceived += port_rx;
            serialPort.PortName = port_name;
        }
        ~serial()
        {
            serialPort.Close();
        }
        public void setport(string com)
        {
            port_name = com;
            serialPort.Close();
            serialPort.PortName = port_name;
            System.Console.WriteLine("setport {0}", port_name);
            connect();
        }
        public void getports()
        {
            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            for (int i = 0; i < ports.Length; ++i) {
                System.Console.WriteLine("port {0}", ports[i]);
            }
        }
        public bool connect() {
            System.Console.WriteLine("try to open port {0}", port_name);
            try {
                serialPort.Open();
            } catch (Exception) {
                System.Console.WriteLine("unable to open port {0}", port_name);
                return false;
            }
            System.Console.WriteLine("Yes!!! port {0} is open", port_name);
            return true;
        }
        public void read() {
            //return serialPort.Read();
        }
        public void write(string s) {
            //byte[] buf = Encoding.ASCII.GetBytes(s);
            //System.Console.WriteLine("sending {0} to serial port", buf);
            //serialPort.Write(buf, 0, s.Length);
            System.Console.WriteLine("sending {0} to serial port", s);
            serialPort.Write(s);
        }
        private void port_rx(object sender, System.IO.Ports.SerialDataReceivedEventArgs e) {
            
            System.Console.WriteLine("got indication");
            if (serialPort.IsOpen == true) {
                // Accumulate all data in a string buffer. Always append, so we never lose any data.
                rx_str += serialPort.ReadExisting();
                int idx;
                while ((idx = rx_str.IndexOf('\n')) >= 0) {
                    System.Console.WriteLine("received: {0}", rx_str.Substring(0, idx));
                    rx_str = rx_str.Substring(idx + 1);
                }
            }
        }
        public string port_name;
        private SerialPort serialPort;
        private string rx_str = string.Empty;
    }

    static class Program {
        [STAThread]
        static void Main() {
            
            proto.serial port  = new proto.serial();
            if (!port.connect()) {
                return;
            }
            port.getports();
            while (true) {
                System.Console.Write("{0}> ", port.port_name);
                string cmd = System.Console.ReadLine();
                System.Console.Write("you entered {0}\n", cmd);
                if (cmd == "") continue;
                if (cmd == "quit") {
                    return;
                }
                else if (cmd == "list") {
                    port.getports();
                }
                else if (cmd.Length > 4 && cmd.Substring(0,3) == "com") {
                    port.setport(cmd);
                }
                else if (cmd.Length > 4 && cmd.Substring(0,3) == "put") {
                    string io_cmd = cmd.Substring(4).ToString();
                    io_cmd += "\n";
                    port.write(io_cmd);
                }
                else if (cmd.Length > 4 && cmd.Substring(0,3) == "get") {
                    port.read();
                }
                else {                    
                    string io_cmd = cmd;
                    io_cmd += "\n";
                    System.Console.Write("writing {0} to port\n", io_cmd);
                    port.write(io_cmd);
                }

            }
        }
    }
}
