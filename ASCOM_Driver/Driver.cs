//tabs=4
// --------------------------------------------------------------------------------
// TODO fill in this information for your driver, then remove this line!
//
// ASCOM Focuser driver for ZeRefractorController
//
// Description:	Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam 
//				nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam 
//				erat, sed diam voluptua. At vero eos et accusam et justo duo 
//				dolores et ea rebum. Stet clita kasd gubergren, no sea takimata 
//				sanctus est Lorem ipsum dolor sit amet.
//
// Implements:	ASCOM Focuser interface version: 1.0
// Author:		(XXX) Your N. Here <your@email.here>
//
// Edit Log:
//
// Date			Who	Vers	Description
// -----------	---	-----	-------------------------------------------------------
// dd-mmm-yyyy	XXX	1.0.0	Initial edit, from ASCOM Focuser Driver template
// --------------------------------------------------------------------------------
//
using System;
using System.Collections;
using System.Runtime.InteropServices;
using ASCOM.DeviceInterface;
using ASCOM.Utilities;
using System.Globalization;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Diagnostics;

namespace ASCOM.ZeRefractorController
{
    //
    // Your driver's ID is ASCOM.ZeRefractorController.Focuser
    //
    // The Guid attribute sets the CLSID for ASCOM.ZeRefractorController.Focuser
    // The ClassInterface/None addribute prevents an empty interface called
    // _Focuser from being created and used as the [default] interface
    //
    [Guid("0a37e997-249f-4fb5-a7c7-d51c27a713d5")]
    [ClassInterface(ClassInterfaceType.None)]
    [ComVisible(true)]
    public class Focuser : IFocuserV2
    {
        #region Constants
        //
        // Driver ID and descriptive string that shows in the Chooser
        //
        private const string driverId = "ASCOM.ZeRefractorController.Focuser";
        // TODO Change the descriptive string for your driver then remove this line
        private const string driverDescription = "ZeRefractorController Focuser";
        //
        private const int ASCOM_SERVER_PORT = 9108;      // my FSQ-106ED serial number ;)

        enum T_HidCommand
        {
            FVERSION = 0x20,
            FABOUT,
            DS,

            FTM,    // Measure temperature sensor 1
            FT2M,   // Measure temperature sensor 2
            FTR,    // Read temperature sensor 1
            FT2R,   // Read temperature sensor 2
            FTZR,   // set temperature compensation status
            FTZL,   // read temperature compensation status
            FTCR,   // read temperature coefficient
            FTCL,   // set temperature coefficient

            FPR,    // read motor position
            FPL,    // set motor new position
            FPD,    // return the direction of the last move
            FPI,    // move motor inside direction
            FPO,    // move motor outside direction
            FPA,    // move motor absolute direction
            FPH,    // halt motor

            FHR,    // read heating power
            FHL,    // set heating power

            FSAR,   // read delay between step auto
            FSAL,   // setup delay between step auto
            FSMR,   // read delay between step manual
            FSML,   // setup delay between step manual
            FSSR,   // read stepper mode
            FSSL,   // setup stepper mode
            FSIR,   // read min position
            FSIL,   // setup min position
            FSOR,   // read max position
            FSOL,   // setup max position
            FSPR,   // read initial position
            FSPL,   // set initial position
            FSPC,   // set current postion as initial position
            FSBR,   // read backlash value
            FSBL,   // set backlash value
            FSCR,   // read config_byte
            FSCL,   // set config_byte

            FPWR,   // read power suply voltage
            FVAR,   // read power suply alarm threshold
            FVAL,   // set power suply alarm threshold

            FHMD,   // read humidity

            FRD     // read EEPROM (debug)
        };

        enum T_ZrcCommand
        {
            IsMoving = 0xC0,
            BrinhToFront,
            CloseZRC
        }
        #endregion

        #region ASCOM Registration
        //
        // Register or unregister driver for ASCOM. This is harmless if already
        // registered or unregistered. 
        //
        private static void RegUnregASCOM(bool bRegister)
        {
            using (var p = new Profile())
            {
                p.DeviceType = "Focuser";
                if (bRegister)
                    p.Register(driverId, driverDescription);
                else
                    p.Unregister(driverId);
            }
        }

        [ComRegisterFunction]
        public static void RegisterASCOM(Type t)
        {
            RegUnregASCOM(true);
        }

        [ComUnregisterFunction]
        public static void UnregisterASCOM(Type t)
        {
            RegUnregASCOM(false);
        }
        #endregion

        #region Implementation of IFocuserV2

        public void SetupDialog()
        {
            using (var f = new SetupDialogForm())
            {
                f.ShowDialog();
            }
        }

        public string Action(string actionName, string actionParameters)
        {
            throw new ASCOM.MethodNotImplementedException("Action");
        }

        public void CommandBlind(string command, bool raw)
        {
            throw new ASCOM.MethodNotImplementedException("CommandBlind");
        }

        public bool CommandBool(string command, bool raw)
        {
            throw new ASCOM.MethodNotImplementedException("CommandBool");
        }

        public string CommandString(string command, bool raw)
        {
            throw new ASCOM.MethodNotImplementedException("CommandString");
        }

        public void Dispose()
        {
        }

        public void Halt()
        {
            ExchangeMessage(T_HidCommand.FPH);
        }

        public void Move(int value)
        {
            int response = 0;

            if (Absolute)
            {
                ExchangeMessage(T_HidCommand.FPA, value, out response);
            }
            else
            {
                if (value > 0)
                {
                    ExchangeMessage(T_HidCommand.FPO, value, out response);
                }
                else if (value < 0)
                {
                    ExchangeMessage(T_HidCommand.FPI, value, out response);
                }
            }

            if (value != response)
            {
                throw new System.InvalidOperationException("value != response");
            }
        }

        private bool m_Connected;

        Process ZeRefractorControllerProcess;

        public bool Connected
        {
            get { return m_Connected; }
            set
            {
                if (value)
                {
                    m_Connected = true;
                    string d = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
#if DEBUG
                        string ZeRefractorControllerPath = @"..\..\..\Debug\ZeRefractorController.exe";
#else
                        string ZeRefractorControllerPath = d + @"\\ZeRefractorController.exe";
#endif
                    try
                    {
                        ProcessStartInfo startInfo = new ProcessStartInfo(ZeRefractorControllerPath);
                        startInfo.WindowStyle = ProcessWindowStyle.Minimized;
                        startInfo.Arguments = "-minimized";
                        ZeRefractorControllerProcess = Process.Start(startInfo);
                    }
                    catch (Exception e)
                    {
                        m_Connected = false;
                        Exception myexp = new Exception(ZeRefractorControllerPath + " : " + e.Message);
                        throw myexp;
                    }
                }
                //else
                //{
                //    byte r;

                //    ExchangeMessage(T_ZrcCommand.CloseZRC, out r);
                //    if (!ZeRefractorControllerProcess.WaitForExit(500))
                //        ZeRefractorControllerProcess.Kill();
                //    m_Connected = false;
                //}
            }
        }

        public string Description
        {
            get { throw new System.NotImplementedException(); }
        }

        public string DriverInfo
        {
            get { throw new System.NotImplementedException(); }
        }

        public string DriverVersion
        {
            get
            {
                Version version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version;
                return String.Format(CultureInfo.InvariantCulture, "{0}.{1}", version.Major, version.Minor);
            }
        }

        public short InterfaceVersion
        {
            get { return 2; }
        }

        public string Name
        {
            get { return "ZeRefractorController"; }
        }

        public ArrayList SupportedActions
        {
            get { return new ArrayList(); }
        }

        public bool Absolute
        {
            get { return true; }
        }

        public bool IsMoving
        {
            get
            {
                byte val;

                ExchangeMessage(T_ZrcCommand.IsMoving, out val);
                bool r = (val != 0);
                return r;
            }
        }

        // use the V2 connected property
        public bool Link
        {
            get { return this.Connected; }
            set { this.Connected = value; }
        }

        public int MaxIncrement
        {
            get { return MaxStep; }
        }

        public int MaxStep
        {
            get
            {
                int val;
                ExchangeMessage(T_HidCommand.FSOR, out val);
                return val;
            }
        }

        public int Position
        {
            get
            {
                int val;

                ExchangeMessage(T_HidCommand.FPR, out val);
                return val;
            }
        }

        public double StepSize
        {
            get { throw new System.NotImplementedException(); }
        }

        public bool TempComp
        {
            get { return false; }
            set { }
        }

        public bool TempCompAvailable
        {
            get { return true; }
        }

        public double Temperature
        {
            get
            {
                int val;

                ExchangeMessage(T_HidCommand.FTR, out val);
                return (double)val / 10;
            }
        }

        #endregion

        #region TCP functions

        private Socket ConnectSocket(string server)
        {
            Socket s = null;
            Socket tempSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            tempSocket.Connect(server, ASCOM_SERVER_PORT);

            if (tempSocket.Connected)
            {
                s = tempSocket;
            }

            return s;
        }

        Byte[] bytesSent;
        Byte[] bytesReceived = new Byte[64];

        private int SocketSendReceive()
        {
            int bytes = 0;

            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            s.Connect("127.0.0.1", ASCOM_SERVER_PORT);

            if (s.Connected)
            {
                s.Send(bytesSent, bytesSent.Length, 0);
                bytes = s.Receive(bytesReceived, bytesReceived.Length, 0);
            }
            else
            {
                m_Connected = false;
            }
            return bytes;
        }

        private bool ExchangeMessage(T_HidCommand Cmd)
        {
            int cr;
            bytesSent = new Byte[1];
            bytesSent[0] = (Byte)Cmd;

            cr = SocketSendReceive();

            if (cr != 0 && bytesReceived[0] == (Byte)Cmd)
            {
                return true;
            }

            return false;
        }

        private bool ExchangeMessage(T_HidCommand Cmd, out int Response)
        {
            int cr;
            bytesSent = new Byte[1];
            bytesSent[0] = (Byte)Cmd;

            cr = SocketSendReceive();

            Response = 0;
            if (cr != 0 && bytesReceived[0] == (Byte)Cmd)
            {
                Response = bytesReceived[1] << 8;
                Response |= bytesReceived[2];
                return true;
            }

            return false;
        }

        private bool ExchangeMessage(T_ZrcCommand Cmd, out byte Response)
        {
            int cr;
            bytesSent = new Byte[1];
            bytesSent[0] = (Byte)Cmd;

            cr = SocketSendReceive();

            Response = 0;
            if (cr != 0 && bytesReceived[0] == (Byte)Cmd)
            {
                Response = bytesReceived[1];
                return true;
            }

            return false;
        }

        private bool ExchangeMessage(T_HidCommand Cmd, Byte Param, out int Response)
        {
            int cr;
            bytesSent = new Byte[2];
            bytesSent[0] = (Byte)Cmd;
            bytesSent[1] = Param;

            cr = SocketSendReceive();

            Response = 0;
            if (cr != 0 && bytesReceived[0] == (Byte)Cmd)
            {
                Response = bytesReceived[1] << 8;
                Response |= bytesReceived[2];
                return true;
            }

            return false;
        }

        private bool ExchangeMessage(T_HidCommand Cmd, int Param, out int Response)
        {
            int cr;
            bytesSent = new Byte[3];
            bytesSent[0] = (Byte)Cmd;
            bytesSent[1] = (Byte)(Param >> 8 & 0xFF);
            bytesSent[2] = (Byte)(Param & 0xFF);

            cr = SocketSendReceive();

            Response = 0;
            if (cr != 0 && bytesReceived[0] == (Byte)Cmd)
            {
                Response = bytesReceived[1] << 8;
                Response |= bytesReceived[2];
                return true;
            }

            return false;
        }

        /*
        bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, unsigned char Param, unsigned char *Response)
        {
            bool cr;
            bufOut[0] = 0x00;
            bufOut[1] = (unsigned char) Cmd;
            bufOut[2] = Param;
            memset((void*)&bufOut[3], 0x00, sizeof(bufOut) - 3);

            cr = ExchangeBuffer();

            if (cr && bufIn[0] == (unsigned char) Cmd)
            {
                *Response = bufIn[1];
                return true;
            }

            return false;
        }

        bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, char **Response)
        {
            bool cr;
            bufOut[0] = 0x00;
            bufOut[1] = (unsigned char) Cmd;
            memset((void*)&bufOut[2], 0x00, sizeof(bufOut) - 2);

            cr = ExchangeBuffer();

            if (cr && bufIn[0] == (unsigned char) Cmd)
            {
                *Response = (char *)bufIn + 1;
                return true;
            }

            return false;
        }

        */

        #endregion
    }
}
