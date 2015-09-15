#include "zerefractorcontroller.h"
#include "hid_pnp.h"
#include <QMessageBox>
#include <QTcpSocket>

void ZeRefractorController::startASCOMServer()
{
    ASCOMServer = new QTcpServer(this);

    if (!ASCOMServer->listen(QHostAddress::LocalHost, ASCOM_SERVER_PORT))
    {
        QMessageBox::critical(this, tr("ZeRefractorController"), tr("Unable to start the ASCOM server: %1.").arg(ASCOMServer->errorString()));
        return;
    }

    connect(ASCOMServer, SIGNAL(newConnection()), this, SLOT(ascom_command()));
}

inline void set_int(int i, unsigned char *buffer)
{
    buffer[1] = (i >> 8) & 0xFF;
    buffer[2] = i & 0xFF;
}

inline int get_int(unsigned char *buffer)
{
    int i;

    i = buffer[1] << 8;
    i |= buffer[2];
    return i;
}

void ZeRefractorController::ascom_command()
{
    ASCOMConnection = ASCOMServer->nextPendingConnection();
    connect(ASCOMConnection, SIGNAL(disconnected()), ASCOMConnection, SLOT(deleteLater()));
    connect(ASCOMConnection, SIGNAL(readyRead()), this, SLOT(read_command()));
    //connect(ASCOMConnection, SIGNAL(error(QAbstractSocket::SocketError)),
}

void ZeRefractorController::read_command()
{
    unsigned char buffer[64];
    int nb_read, nb_write;
    nb_read = ASCOMConnection->read((char *)buffer, 64);

    nb_write = 0;
    if (nb_read != 0)
    {
        if (buffer[0] < 0xC0)
        {
            HID_PnP::T_HidCommand Cmd = (HID_PnP::T_HidCommand) buffer[0];
            int i;

            switch (Cmd)
            {
            case HID_PnP::FTR :
            case HID_PnP::FPR :
            case HID_PnP::FSIR :
            case HID_PnP::FSOR :
                UsbHid->ExchangeMessage(Cmd, &i);
                set_int(i, buffer);
                nb_write = 3;
                break;

            case HID_PnP::FPI :
                i = get_int(buffer);
                destPostion = Position - i;
                UsbHid->ExchangeMessage(Cmd, i, &i);
                set_int(i, buffer);
                nb_write = 3;
                break;

            case HID_PnP::FPO :
                i = get_int(buffer);
                destPostion = Position + i;
                UsbHid->ExchangeMessage(Cmd, i, &i);
                set_int(i, buffer);
                nb_write = 3;
                break;

            case HID_PnP::FPA :
                i = get_int(buffer);
                destPostion = i;
                UsbHid->ExchangeMessage(Cmd, i, &i);
                set_int(i, buffer);
                nb_write = 3;
                break;

            case HID_PnP::FPH :
                UsbHid->ExchangeMessage(Cmd);
                nb_write = 1;
                break;
            }
        }
        else
        {
            T_ZrcCommand Cmd = (T_ZrcCommand) buffer[0];
            //int i;
            
            switch (Cmd)
            {
            case IsMoving :
                buffer[1] = isMoving ? 1 : 0;
                nb_write = 2;
                break;

            case BringToFront :
                this->show();
                this->setWindowState(Qt::WindowActive);
                nb_write = 1;
                break;

            case SingleInstance :
                if (!systrayIcon->isVisible())
                {
                    this->setFocus();
                    this->raise();
                    this->setWindowState(Qt::WindowActive);
                }
                else
                {
                    systrayIcon->showMessage("Ze Refractor Controller",tr("I'm here."));
                }
                nb_write = 1;
                break;

            case CloseZRC :
                this->close();
                nb_write = 1;
                break;
            }
        }
    }
    
    if (nb_write == 0)
    {
        buffer[0] = 0xFF;
        nb_write = 1;
    }

    ASCOMConnection->write((char *)buffer, nb_write);
    ASCOMConnection->disconnectFromHost();
}