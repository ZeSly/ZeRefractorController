#ifndef HID_PNP_H
#define HID_PNP_H

#include <QObject>
#include <QTimer>
#include "../HIDAPI/hidapi.h"

#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STR 65

class HID_PnP : public QObject
{
    Q_OBJECT
public:
    explicit HID_PnP(QObject *parent = 0);
    ~HID_PnP();
    void toggle_leds();

    enum T_HidCommand
    {
        FVERSION = 0x20,
        FABOUT,
        DS,

        FTN,    // read sensor number 
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

        FIRMWARE,

        FRD     // read EEPROM (debug)
    };

signals:
    void hid_comm_update(bool isConnected);

private:
    hid_device *device;
    bool isConnected;
    unsigned char bufIn[MAX_STR];
    unsigned char bufOut[MAX_STR];
    bool ExchangeBuffer();

    void CloseDevice();

public:
    bool ExchangeMessage(T_HidCommand Cmd);
    bool ExchangeMessage(T_HidCommand Cmd, int *Response);
    bool ExchangeMessage(T_HidCommand Cmd, char Param, int *Response);
    bool ExchangeMessage(T_HidCommand Cmd, int Param, int *Response);
    bool ExchangeMessage(T_HidCommand Cmd, unsigned char Param, unsigned char *Response);
    bool ExchangeMessage(T_HidCommand Cmd, char **Response);
    //void ToSocket(unsigned char *bytesSent, int l, unsigned char **bytesReceived);
};

#endif // HID_PNP_H
