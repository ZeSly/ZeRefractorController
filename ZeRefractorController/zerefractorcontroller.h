#ifndef ZEREFRACTORCONTROLLER_H
#define ZEREFRACTORCONTROLLER_H

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTcpServer>
#include <QSignalMapper>

#include "hid_pnp.h"

#define ASCOM_SERVER_PORT 9108      // my FSQ-106ED serial number ;)


namespace Ui {
class ZeRefractorController;
}

class ZeRefractorController : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ZeRefractorController(QWidget *parent = 0);
    ~ZeRefractorController();

public slots:
    void update_gui(bool isConnected);
    void ascom_command();
    void read_command();
    void blinking();

protected:
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void on_pushButtonToggleLED_clicked();
    void on_timerTemperature();
    void on_timerMotor();
    void systrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void on_pushButton_moveIn_clicked();
    void on_pushButton_moveOut_clicked();
    void on_pushButton_moveAbsolute_clicked();
    void on_tabWidget_currentChanged(int index);
    void on_pushButtonSave_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_pushButton_logBrowse_clicked();
    void on_checkBox_enableLog_stateChanged(int arg1);

    void on_pushButtonSteCurrentPosition_clicked();
    void on_pushButtonFirmwareUpdate_clicked();
    void on_pushButton_addPosition_clicked();
    void on_pushButton_removePosition_clicked();

    void on_GoPushButton_clicked(int row);

    void on_tableWidget_Positons_cellChanged(int row, int column);

private:
    Ui::ZeRefractorController *ui;
    HID_PnP *UsbHid;
    QTimer *timerTemperature;
    QTimer *timerMotor;
    QTimer *blinkingTimer;
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *systrayIcon;
    QMenu *systrayMenu;
    QSignalMapper *positionsSignalMapper;

    bool measureTemperature;
    bool isMoving;
    int Position;
    int destPostion;
    void setIsMoving(bool m);

    double Temperature, Humidity;
    void Log(bool Immediat);

    void createActions();
    void createSystrayIcon();

    QTcpServer *ASCOMServer;
    QTcpSocket *ASCOMConnection;
    void startASCOMServer();

    bool PositionsPopulated;

public :
    enum T_ZrcCommand
    {
        IsMoving = 0xC0,
        BringToFront,
        SingleInstance,
        CloseZRC
    };
};

#endif // ZEREFRACTORCONTROLLER_H
