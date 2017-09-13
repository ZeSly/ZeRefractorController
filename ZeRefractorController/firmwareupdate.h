#ifndef FIRMWAREUPDATE_H
#define FIRMWAREUPDATE_H

#include <QDialog>
#include <QFuture>
#include "ui_firmwareupdate.h"

#include "Comm.h"
#include "DeviceData.h"
#include "Device.h"
#include "ImportExportHex.h"

class FirmwareUpdate : public QDialog
{
    Q_OBJECT

public:
    FirmwareUpdate(QString hexFile, QWidget *parent = 0);
    ~FirmwareUpdate();

    void GetQuery(void);
    void LoadFile(QString newFileName);

    void EraseDevice(void);
    void BlankCheckDevice(void);
    void WriteDevice(void);
    void VerifyDevice(void);

    void setBootloadBusy(bool busy);

signals:
    void IoWithDeviceCompleted(QString msg, Comm::ErrorCode, double time);
    void IoWithDeviceStarted(QString msg);
    void AppendString(QString msg);
    void SetProgressBar(int newValue);

public slots:
    void Connection(void);
    void IoWithDeviceComplete(QString msg, Comm::ErrorCode, double time);
    void IoWithDeviceStart(QString msg);
    void AppendStringToTextbox(QString msg);
    void UpdateProgressBar(int newValue);

protected:
    Comm* comm;
    DeviceData* deviceData;
    DeviceData* hexData;
    Device* device;

    QFuture<void> future;

    QString fileName, watchFileName;
    QTimer *timer;

    bool writeFlash;
    bool writeEeprom;
    bool writeConfig;
    bool eraseDuringWrite;
    bool hexOpen;

    //void setBootloadEnabled(bool enable);

    Comm::ErrorCode RemapInterruptVectors(Device* device, DeviceData* deviceData);

    void closeEvent(QCloseEvent *e);

private:
    Ui::FirmwareUpdate *ui;
    QString hexFile;

    int failed;

    bool wasBootloaderMode;

//private slots:
//    void on_actionBlank_Check_triggered();
//    void on_actionReset_Device_triggered();
//    void on_action_Settings_triggered();
//    void on_action_Verify_Device_triggered();
//    void on_action_About_triggered();
//    void on_actionWrite_Device_triggered();
//    void on_actionErase_Device_triggered();
//    void on_actionExit_triggered();
};

#endif // FIRMWAREUPDATE_H
