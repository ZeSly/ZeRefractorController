#include "hid_pnp.h"

HID_PnP::HID_PnP(QObject *parent) : QObject(parent) {
    isConnected = false;
    device = NULL;
    memset(bufIn, 0x00, sizeof(bufIn));
    memset(bufOut, 0x00, sizeof(bufOut));
}

HID_PnP::~HID_PnP()
{
    hid_close(device);
    device = NULL;
}

void HID_PnP::toggle_leds()
{
    bufOut[0] = 0x00;
    bufOut[1] = 0x80;
    memset((void*)&bufOut[2], 0x00, sizeof(bufOut) - 2);

    if (hid_write(device, bufOut, sizeof(bufOut)) == -1)
    {
        CloseDevice();
        return;
    }
}

void HID_PnP::CloseDevice()
{
    hid_close(device);
    device = NULL;
    isConnected = false;
    memset(bufIn, 0x00, sizeof(bufIn));
    memset(bufOut, 0x00, sizeof(bufOut));
    hid_comm_update(isConnected);
}

bool HID_PnP::ExchangeBuffer()
{
    if (isConnected == false)
    {
        device = hid_open(0x04d8, 0x0040, NULL);

        if (device)
        {
            isConnected = true;
            hid_set_nonblocking(device, false);
            hid_comm_update(isConnected);
        }
        else
        {
            hid_comm_update(isConnected);
            return false;
        }
    }

    if (hid_write(device, bufOut, sizeof(bufOut)) == -1)
    {
        CloseDevice();
        return false;
    }

    if (hid_read(device, bufIn, sizeof(bufIn))  == -1)
    {
        CloseDevice();
        return false;
    }

    return true;
}

bool HID_PnP::ExchangeMessage(T_HidCommand Cmd)
{
    bool cr;
    bufOut[0] = 0x00;
    bufOut[1] = (unsigned char) Cmd;
    memset((void*)&bufOut[2], 0x00, sizeof(bufOut) - 2);

    cr = ExchangeBuffer();

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        return true;
    }

    return false;
}

bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, int *Response)
{
    bool cr;
    bufOut[0] = 0x00;
    bufOut[1] = (unsigned char) Cmd;
    memset((void*)&bufOut[2], 0x00, sizeof(bufOut) - 2);

    cr = ExchangeBuffer();

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        *Response = bufIn[2] << 8;
        *Response |= bufIn[3];
        return true;
    }

    return false;
}

bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, char Param, int *Response)
{
    bool cr;
    bufOut[0] = 0x00;
    bufOut[1] = (unsigned char) Cmd;
    bufOut[2] = Param;
    memset((void*)&bufOut[3], 0x00, sizeof(bufOut) - 3);

    cr = ExchangeBuffer();

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        *Response = bufIn[2] << 8;
        *Response |= bufIn[3];
        return true;
    }

    return false;
}

bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, int Param, int *Response)
{
    bool cr;
    bufOut[0] = 0x00;
    bufOut[1] = (unsigned char) Cmd;
    bufOut[2] = (unsigned char)(Param >> 8 & 0xFF);
    bufOut[3] = (unsigned char)(Param & 0xFF);
    memset((void*)&bufOut[4], 0x00, sizeof(bufOut) - 4);

    cr = ExchangeBuffer();

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        *Response = bufIn[2] << 8;
        *Response |= bufIn[3];
        return true;
    }

    return false;
}

bool HID_PnP::ExchangeMessage(T_HidCommand Cmd, unsigned char Param, unsigned char *Response)
{
    bool cr;
    bufOut[0] = 0x00;
    bufOut[1] = (unsigned char) Cmd;
    bufOut[2] = Param;
    memset((void*)&bufOut[3], 0x00, sizeof(bufOut) - 3);

    cr = ExchangeBuffer();

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        *Response = bufIn[2];
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

    if (cr && bufIn[1] == (unsigned char) Cmd)
    {
        *Response = (char *)bufIn + 2;
        return true;
    }

    return false;
}

//void HID_PnP::ToSocket(unsigned char *bytesSent, int l, unsigned char **bytesReceived)
//{
//    bool cr;
//    bufOut[0] = 0x00;
//    memcpy((void*)&bufOut[1], bytesSent, sizeof(bufOut) - 1);
//
//    cr = ExchangeBuffer();
//
//}
