using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.IO;
using System.Text;
using System.Diagnostics;

namespace proto {
    public class Program {
        const string path = "\\\\.\\Modern";

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern IntPtr CreateFile(string lpFileName, uint dwDesiredAccess, uint dwShareMode, IntPtr lpSecurityAttributes, uint dwCreationDisposition, uint dwFlagsAndAttributes, IntPtr hTemplateFile);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern bool DeviceIoControl(IntPtr hDevice, uint dwIoControlCode, IntPtr lpInBuffer, int nInBufferSize, IntPtr lpOutBuffer,
                                            int nOutBufferSize, ref int lpBytesReturned, IntPtr lpOverlapped);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern int CloseHandle(IntPtr hObject);

        internal const uint GENERIC_READ = 0x80000000;
        internal const uint GENERIC_WRITE = 0x40000000;
        internal const uint OPEN_EXISTING = 3;

        private const UInt32 FILEIO_TYPE = 40001;
        public const UInt32 METHOD_BUFFERED = 0x00;
        public const UInt32 FILE_ANY_ACCESS = 0x00;
        public const UInt32 FILE_READ_ACCESS = 0x01;
        
        internal static UInt32 CTL_CODE(UInt32 DeviceType, UInt32 Function, UInt32 Method, UInt32 Access)
        {
            return ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method);
        }

        private static UInt32 IOCTL_MODERN_START_THREAD  = CTL_CODE(FILEIO_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS);
        private static UInt32 IOCTL_MODERN_STOP_THREAD   = CTL_CODE(FILEIO_TYPE, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS);
        private static UInt32 IOCTL_MODERN_QUEUE_REQUEST = CTL_CODE(FILEIO_TYPE, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS);

        public static void doIO(IntPtr hDevice, UInt32 ioctl) {
            UInt32 retVal = 0;
            IntPtr outBuffer = Marshal.AllocHGlobal(sizeof(UInt32));
            IntPtr inBuffer = Marshal.AllocHGlobal(sizeof(UInt64));

            int dwBytes = 0;
            bool ret = DeviceIoControl(hDevice, ioctl, inBuffer, sizeof(UInt64), outBuffer, sizeof(UInt32), ref dwBytes, IntPtr.Zero);
            if (ret == false)
            {
                int error = Marshal.GetLastWin32Error();
                if (error != 0)
                {
                    Console.WriteLine("Unexpected error: 0x{0:x}", error);
                    throw new Win32Exception(error);
                }
            }

            retVal = (UInt32)Marshal.PtrToStructure(outBuffer, typeof(UInt32));

            Marshal.FreeHGlobal(inBuffer);
            Marshal.FreeHGlobal(outBuffer);
        }

        static void Main() {
            string line;
            IntPtr hDevice;// { get; private set; }

            try {
                hDevice = CreateFile(@"\\.\Modern", GENERIC_WRITE | GENERIC_READ, 0, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero);
                Console.Write("sweet!!");
                
                while (true) {
                    Console.Write("$ ");
                    line = Console.ReadLine();
                    if (line == "quit") {
                        return;
                    }
                    else if (line == "start") {
                        Console.WriteLine("start");
                        Program.doIO(hDevice, IOCTL_MODERN_START_THREAD);
                    }
                    else if (line == "stop") {
                        Console.WriteLine("stop");
                        doIO(hDevice, IOCTL_MODERN_STOP_THREAD);
                    }
                    else if (line == "send") {
                        Console.WriteLine("send");
                        doIO(hDevice, IOCTL_MODERN_QUEUE_REQUEST);
                    }
                }
            } catch (Exception ex) {
                Console.Write("bummer!!");
                Console.WriteLine(ex.ToString());
            }
        }
    }
}
