#include "zerefractorcontroller.h"
#include <QApplication>
#include <QTcpSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ZeSly");
    QCoreApplication::setOrganizationDomain("zesly.net");
    QCoreApplication::setApplicationName("Ze Refractor Controller");

    QTcpSocket *ASCOMConnection = new QTcpSocket();
    ASCOMConnection->connectToHost("127.0.0.1", ASCOM_SERVER_PORT);
    if (ASCOMConnection->waitForConnected(500))
    {
        char b = (unsigned char) ZeRefractorController::SingleInstance;
        ASCOMConnection->write(&b, 1);
        ASCOMConnection->waitForDisconnected(500);
        return 1;
    }

    ZeRefractorController w;
    return a.exec();
}
