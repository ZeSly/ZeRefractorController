#include "zerefractorcontroller.h"
#include "ui_zerefractorcontroller.h"
#include <stdio.h>
#include <math.h>
#include <QHideEvent>
#include <QFileDialog>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QPalette>

#include "firmwareupdate.h"

ZeRefractorController::ZeRefractorController(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ZeRefractorController),
    PositionsPopulated(false)
{
    ui->setupUi(this);
    createActions();
    createSystrayIcon();
    ui->comboBox_stepper_mode->addItem(" ");
    ui->comboBox_stepper_mode->addItem("Half step");
    ui->comboBox_stepper_mode->addItem("Full step");
    ui->comboBox_stepper_mode->addItem("Full step low-co");
    ui->tabWidget->setCurrentIndex(0);
    ui->frameTemperatureGraph->setFormat(QString::fromUtf8("%1""\xC2\xB0""C"));
    ui->frameHumidityGraph->setFormat(QString("%1%"));
    ui->frameHumidityGraph->setLabelPrecison(10);
    ui->tableWidget_Positons->setColumnWidth(2, 32);
    
    positionsSignalMapper = new QSignalMapper(this);
    connect(positionsSignalMapper, SIGNAL(mapped(int)), this, SLOT(on_GoPushButton_clicked(int)));
    
    UsbHid = new HID_PnP();

    connect(UsbHid, SIGNAL(hid_comm_update(bool)), this, SLOT(update_gui(bool)));

    timerTemperature = new QTimer(this);
    timerMotor = new QTimer(this);
    blinkingTimer = new QTimer(this);

    timerTemperature->setSingleShot(true);

    connect(timerTemperature, SIGNAL(timeout()), this, SLOT(on_timerTemperature()));
    connect(timerMotor, SIGNAL(timeout()), this, SLOT(on_timerMotor()));
    connect(blinkingTimer, SIGNAL(timeout()), this, SLOT(blinking()));

    measureTemperature = true;

    Position = 0;
    destPostion = 0;
    isMoving = false;

    QValidator *validator_3digits = new QIntValidator(0, 999, this);
    ui->lineEdit_StepModeAuto->setValidator(validator_3digits);
    ui->lineEdit_StepModeManual->setValidator(validator_3digits);
    ui->lineEdit_Backlash->setValidator(validator_3digits);
    ui->lineEdit_TemperatureCoeff->setValidator(validator_3digits);
    QValidator *validator_6digits = new QIntValidator(0, 999999, this);
    ui->lineEdit_MinPosition->setValidator(validator_6digits);
    ui->lineEdit_MaxPosition->setValidator(validator_6digits);
    ui->lineEdit_InitialPosition->setValidator(validator_6digits);
    ui->lineEdit_LowPowerAlarm->setValidator(new QDoubleValidator(0,99.9,1, this));

    QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    ui->checkBox_enableLog->setChecked(settings.value("EnableLog").toBool());
    ui->lineEdit_logPath->setText(settings.value("LogPath").toString());

    int nb_position = settings.beginReadArray("Positions");
    for (int i = 0 ; i < nb_position ; i++)
    {
        settings.setArrayIndex(i);

        int new_row = ui->tableWidget_Positons->rowCount() + 1;
        ui->tableWidget_Positons->setRowCount(new_row);
        ui->tableWidget_Positons->update();

        QTableWidgetItem *newItem = new QTableWidgetItem(settings.value("Name").toString());
        ui->tableWidget_Positons->setItem(new_row - 1, 0, newItem);

        newItem = new QTableWidgetItem(settings.value("Position").toString());
        ui->tableWidget_Positons->setItem(new_row - 1, 1, newItem);

        QPushButton *newGoPushButton = new QPushButton(tr("Go"),this);
        positionsSignalMapper->setMapping(newGoPushButton, new_row - 1);
        connect(newGoPushButton, SIGNAL(clicked()), positionsSignalMapper, SLOT(map()));
        ui->tableWidget_Positons->setCellWidget(new_row - 1, 2, newGoPushButton);
    }
    settings.endArray();
    PositionsPopulated = true;

    timerMotor->start(250);

    startASCOMServer();

    QStringList args = QCoreApplication::arguments();
    if (args.indexOf(QString("-minimized")) >= 0)
    {
        systrayIcon->show();
        systrayIcon->showMessage("Ze Refractor Controller",tr("Ze Refractor Controller started."));
    }
    else
    {
        show();
    }
}

ZeRefractorController::~ZeRefractorController()
{
    delete UsbHid;
    delete ui;
}

void ZeRefractorController::createActions()
{
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void ZeRefractorController::createSystrayIcon()
{
    systrayMenu = new QMenu();
    systrayMenu->addAction(restoreAction);
    systrayMenu->addSeparator();
    systrayMenu->addAction(quitAction);
    systrayMenu->setDefaultAction(restoreAction);

    systrayIcon = new QSystemTrayIcon(this);
    systrayIcon->setIcon(QIcon(":/ZeRefractorController/Resources/TK004.png"));
    systrayIcon->setContextMenu(systrayMenu);

    connect(systrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(systrayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void ZeRefractorController::systrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        this->show();
        this->setWindowState(Qt::WindowActive);
    }
}

void ZeRefractorController::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent *e = (QWindowStateChangeEvent*)event;
        // make sure we only do this for minimize events
        if ((e->oldState() != Qt::WindowMinimized) && isMinimized())
        {
            QTimer::singleShot(0, this, SLOT(hide()));
            event->ignore();
            systrayIcon->show();
            systrayIcon->showMessage("Ze Refractor Controller","Ze Refractor Controller is still there.");
        }
    }
}

void ZeRefractorController::showEvent(QShowEvent *event)
{
    systrayIcon->hide();
}


void ZeRefractorController::update_gui(bool isConnected)
{
    static bool wasConnected = false;

    if(isConnected)
    {
        if (!wasConnected)
        {
            measureTemperature = true;
            timerTemperature->start(100);
        }
        ui->pushButtonToggleLED->setEnabled(true);
        ui->pushButtonSave->setEnabled(true);
        ui->lcdPosition->setEnabled(true);
        ui->lcdPosition2->setEnabled(true);
        ui->lcdTemperature->setEnabled(true);
        ui->groupBox_relativeMove->setEnabled(true);
        ui->groupBox_absoluteMove->setEnabled(true);
        ui->horizontalSlider->setEnabled(true);
    }
    else
    {
        timerTemperature->stop();
        ui->pushButtonToggleLED->setEnabled(false);
        ui->pushButtonSave->setEnabled(false);
        ui->lcdPosition->setEnabled(false);
        ui->lcdPosition2->setEnabled(false);
        ui->lcdTemperature->setEnabled(false);
        ui->groupBox_relativeMove->setEnabled(false);
        ui->groupBox_absoluteMove->setEnabled(false);
        ui->horizontalSlider->setEnabled(false);
    }
    wasConnected = isConnected;
}

void ZeRefractorController::on_pushButtonToggleLED_clicked()
{
    UsbHid->toggle_leds();
}

void ZeRefractorController::on_timerTemperature()
{
    if (measureTemperature)
    {
        UsbHid->ExchangeMessage(HID_PnP::FTM);
        timerTemperature->start(1900);
        measureTemperature = false;
    }
    else
    {
        int t;
		int16_t tt;
        QLocale locale;

        UsbHid->ExchangeMessage(HID_PnP::FTR, &t);
		tt = (int16_t)t;
        Temperature = (double)tt / 10;
        ui->lcdTemperature->display(QString("%1").arg(Temperature, 0, 'f', 1));
        //ui->lcdTemperature->display(Temperature);
        ui->label_WeatherTemperature->setText(QString::fromUtf8("%1""\xC2\xB0""C").arg(Temperature));
        ui->frameTemperatureGraph->addValue(Temperature);
        timerTemperature->start(100);
        measureTemperature = true;

        // Calculating dew point base on wikipedia
        double a = 17.27;
        double b = 237.7;
        double alpha = (a * Temperature) / (b + Temperature) + log(Humidity / 100.0);
        double dew_point = (b * alpha) / (a - alpha);
        ui->label_WeatherDewPoint->setText(QString::fromUtf8("%1""\xC2\xB0""C").arg(dew_point, 0, 'f', 1));

        static bool dew_point_alerted = false;
        if (dew_point > Temperature)
        {
            if (!dew_point_alerted)
            {
                systrayIcon->show();
                systrayIcon->showMessage("Ze Refractor Controller",tr("Warning: current temperature below dew point."), QSystemTrayIcon::Warning);
                dew_point_alerted = true;
            }

            blinkingTimer->start(500);

            if (ui->checkBox_automaticPower->isChecked())
            {
                int i;
                int value = ui->horizontalSlider->value();

                UsbHid->ExchangeMessage(HID_PnP::FHL,value * 1023 / 100, &i);
                ui->progressBar_heatingPower->setValue(value);
            }
        }
        else if (dew_point_alerted == true)
        {
            dew_point_alerted = false;
            blinkingTimer->stop();

            QPalette palette = ui->label_WeatherDewPoint->palette();
            palette.setColor(QPalette::Window, QColor(0,0,0));
            ui->label_WeatherDewPoint->setPalette(palette);

            if (ui->checkBox_automaticPower->isChecked())
            {
                int i;

                UsbHid->ExchangeMessage(HID_PnP::FHL,0, &i);
                ui->progressBar_heatingPower->setValue(0);
            }
        }
    }

    int h;

    if (UsbHid->ExchangeMessage(HID_PnP::FHMD, &h))
    {
        Humidity = (double)h;
        ui->label_WeatherHumidity->setText(QString("%1%").arg(Humidity));
        ui->frameHumidityGraph->addValue(Humidity);
    }

    if (measureTemperature) Log(false);

    if (ui->tabWidget->currentWidget() == ui->tab_Setup)
    {
        double power_supply;
        int pwr;

        UsbHid->ExchangeMessage(HID_PnP::FPWR, &pwr);
        power_supply = (double)pwr / 1000;
        ui->label_power_supply_value->setText(QString("%1").arg((power_supply)));
    }

}

void ZeRefractorController::blinking()
{
    static bool blink = true;

    QPalette palette = ui->label_WeatherDewPoint->palette();
    if (blink)
        palette.setColor(QPalette::Window, QColor(128,0,0));
    else
        palette.setColor(QPalette::Window, QColor(0,0,0));
    blink = !blink;
    ui->label_WeatherDewPoint->setPalette(palette);
}

void ZeRefractorController::on_timerMotor()
{
    if (UsbHid->ExchangeMessage(HID_PnP::FPR, &Position))
    {
        if (ui->tabWidget->currentWidget() == ui->tab_Focus)
        {
            ui->lcdPosition->display(Position);
        }
        else if (ui->tabWidget->currentWidget() == ui->tab_Positions)
        {
            ui->lcdPosition2->display(Position);
        }

        if (isMoving && Position == destPostion)
        {
            setIsMoving(false);
        }
    }
}

void ZeRefractorController::setIsMoving(bool m)
{
    isMoving = m;
    if (m)
    {
        //isMoving = true;
        ui->pushButton_moveIn->setEnabled(false);
        ui->pushButton_moveOut->setEnabled(false);
        ui->pushButton_moveAbsolute->setText(tr("STOP"));
    }
    else
    {
        isMoving = false;
        Log(true);
        ui->pushButton_moveIn->setEnabled(true);
        ui->pushButton_moveOut->setEnabled(true);
        ui->pushButton_moveAbsolute->setText(tr("GO"));
    }
}

void ZeRefractorController::on_pushButton_moveIn_clicked()
{
    int move = ui->spinBox_relative->value();

    if (move != 0)
    {
        UsbHid->ExchangeMessage(HID_PnP::FPI, move, &move);
        destPostion = Position - move;
        setIsMoving(true);
    }
}

void ZeRefractorController::on_pushButton_moveOut_clicked()
{
    int move = ui->spinBox_relative->value();

    if (move != 0)
    {
        UsbHid->ExchangeMessage(HID_PnP::FPO, move, &move);
        destPostion = Position + move;
        setIsMoving(true);
    }
}

void ZeRefractorController::on_pushButton_moveAbsolute_clicked()
{
    if (!isMoving)
    {
        int move = ui->spinBox_absolute->value();

        destPostion = move;
        UsbHid->ExchangeMessage(HID_PnP::FPA, move, &move);
        setIsMoving(true);
    }
    else
    {
        UsbHid->ExchangeMessage(HID_PnP::FPH);
        setIsMoving(false);
    }
}

void ZeRefractorController::on_tabWidget_currentChanged(int index)
{
    int val;

    if (index == ui->tabWidget->indexOf(ui->tab_Positions))
    {
        ui->tableWidget_Positons->selectedItems().clear();
    }
    else if (index == ui->tabWidget->indexOf(ui->tab_Setup))
    {
        if (UsbHid->ExchangeMessage(HID_PnP::FSSR, &val))
        {
            val >>= 8;
            ui->comboBox_stepper_mode->setCurrentIndex(val);
        }

        if (UsbHid->ExchangeMessage(HID_PnP::FSAR, &val))
            ui->lineEdit_StepModeAuto->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FSMR, &val))
            ui->lineEdit_StepModeManual->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FSIR, &val))
            ui->lineEdit_MinPosition->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FSOR, &val))
            ui->lineEdit_MaxPosition->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FSPR, &val))
            ui->lineEdit_InitialPosition->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FSBR, &val))
            ui->lineEdit_Backlash->setText(QString("%1").arg(val));

        if (UsbHid->ExchangeMessage(HID_PnP::FTCR, &val))
            ui->lineEdit_TemperatureCoeff->setText(QString("%1").arg(val));

        double power_supply;
        QLocale locale;
        if (UsbHid->ExchangeMessage(HID_PnP::FPWR, &val))
        {
            power_supply = (double)val / 1000;
            ui->label_power_supply_value->setText(locale.toString(power_supply));
        }

        if (UsbHid->ExchangeMessage(HID_PnP::FVAR, &val))
        {
            double alarm = (double)val / 10;
            ui->lineEdit_LowPowerAlarm->setText(locale.toString(alarm));
        }

        char *version;
        if (UsbHid->ExchangeMessage(HID_PnP::FVERSION, &version))
            ui->label_ver->setText(QString("Firmware %1").arg(version));
        
        val = 0;
        if (UsbHid->ExchangeMessage(HID_PnP::FSCR , &val))
        {
            val >>= 8;
            ui->checkBox_InvertMotor->setChecked((val & 2) != 0 ? true : false);
            ui->checkBox_PositiveDeltaTempMoveIn->setChecked((val & 4) != 0 ? true : false);
            ui->checkBox_KeepPowerMotoStopped->setChecked((val & 8) != 0 ? true : false);
        }
    }
}

void ZeRefractorController::on_pushButtonSave_clicked()
{
    int val;
    unsigned char bval;

    bval = ui->comboBox_stepper_mode->currentIndex();
    if (bval >= 1 && bval <= 3)
    {
        if (UsbHid->ExchangeMessage(HID_PnP::FSSL, bval, &bval))
            ui->comboBox_stepper_mode->setCurrentIndex(bval);
    }

    val = ui->lineEdit_StepModeAuto->text().toInt();    
    if (UsbHid->ExchangeMessage(HID_PnP::FSAL, val, &val))
        ui->lineEdit_StepModeAuto->setText(QString("%1").arg(val));

    val = ui->lineEdit_StepModeManual->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FSML, val, &val))
        ui->lineEdit_StepModeManual->setText(QString("%1").arg(val));

    val = ui->lineEdit_MinPosition->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FSIL, val, &val))
        ui->lineEdit_MinPosition->setText(QString("%1").arg(val));

    val = ui->lineEdit_MaxPosition->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FSOL, val, &val))
        ui->lineEdit_MaxPosition->setText(QString("%1").arg(val));

    val= ui->lineEdit_InitialPosition->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FSPL, val, &val))
        ui->lineEdit_InitialPosition->setText(QString("%1").arg(val));

    val = ui->lineEdit_Backlash->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FSBL, val, &val))
        ui->lineEdit_Backlash->setText(QString("%1").arg(val));

    val = ui->lineEdit_TemperatureCoeff->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FTCL, val, &val))
        ui->lineEdit_TemperatureCoeff->setText(QString("%1").arg(val));

    QLocale locale;
    bool double_ok;
    double alarm =  locale.toDouble(ui->lineEdit_LowPowerAlarm->text(), &double_ok);
    if (double_ok)
    {
        val = alarm * 10;
        if (UsbHid->ExchangeMessage(HID_PnP::FVAL, val, &val))
        {
            alarm = (double) val / 10;
            ui->lineEdit_LowPowerAlarm->setText(QString("%1").arg(alarm));
        }
    }

    bval = 0;
    if (ui->checkBox_InvertMotor->isChecked())             bval |= 2;
    if (ui->checkBox_PositiveDeltaTempMoveIn->isChecked()) bval |= 4;
    if (ui->checkBox_KeepPowerMotoStopped->isChecked())    bval |= 8;
    if (UsbHid->ExchangeMessage(HID_PnP::FSCL, bval, &bval))
    {
        ui->checkBox_InvertMotor->setChecked((bval & 2) != 0 ? true : false);
        ui->checkBox_PositiveDeltaTempMoveIn->setChecked((bval & 4) != 0 ? true : false);
        ui->checkBox_KeepPowerMotoStopped->setChecked((bval & 8) != 0 ? true : false);
    }

    QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("EnableLog", ui->checkBox_enableLog->isChecked());
    settings.setValue("LogPath", ui->lineEdit_logPath->text());
}

void ZeRefractorController::on_horizontalSlider_valueChanged(int value)
{
    int i;
    
    if (!ui->checkBox_automaticPower->isChecked())
    {
        UsbHid->ExchangeMessage(HID_PnP::FHL,value * 1023 / 100, &i);
        ui->progressBar_heatingPower->setValue(value);
    }
}

void ZeRefractorController::on_checkBox_enableLog_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->lineEdit_logPath->setEnabled(true);
    }
    else
    {
        ui->lineEdit_logPath->setDisabled(true);
    }
}

void ZeRefractorController::on_pushButton_logBrowse_clicked()
{
    if (ui->checkBox_enableLog->isChecked())
    {
        QString h = QDir::homePath();
        QString t = ui->lineEdit_logPath->text();
        QDir log_dir(t);
        if (t.isEmpty() || !log_dir.exists())
        {
            t = QFileDialog::getExistingDirectory(this, tr("Log directory"), h);
        }
        else
        {
            t = QFileDialog::getExistingDirectory(this, tr("Log directory"), t);
        }

        if (!t.isEmpty())
            ui->lineEdit_logPath->setText(t);
    }
}

void ZeRefractorController::Log(bool Immediat)
{
    static QTime t;

    if (Immediat || t.elapsed() >= 5*60*1000)   // 5 minutes
    {
        QDateTime dateFile = QDateTime::currentDateTime().addSecs(-3600*12);
        QString sLogFile = "ZeRefractorController_" + dateFile.toString("yyyyMMdd") + ".csv";
        QString sLogPath = ui->lineEdit_logPath->text();

        if (!sLogPath.endsWith(QDir::separator()))
        {
            sLogPath += QDir::separator();
        }

        QFile logFile(sLogPath + sLogFile);
        bool logExist = logFile.exists();

        if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QTextStream logStream(&logFile);

            if (!logExist)
            {
                logStream << "Date;Position;Temperature;Humidity" << endl ;
            }
            logStream << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate) << ";";
            logStream << QString("%1").sprintf("%6d",Position) << ";";
            logStream << QString("%1").arg(Temperature, 5, 'f', 1) << ";";
            logStream << QString("%1").arg(Humidity) << endl;

            logFile.close();
            t.start();
        }
    }
}

void ZeRefractorController::on_pushButtonSteCurrentPosition_clicked()
{
    int val;

    val= ui->lineEdit_CurrentPosition->text().toInt();
    if (UsbHid->ExchangeMessage(HID_PnP::FPL, val, &val))
        ui->lineEdit_CurrentPosition->setText(QString("%1").arg(val));

}

void ZeRefractorController::on_pushButtonFirmwareUpdate_clicked()
{
    QString msg, newFileName;
    QTextStream stream(&msg);

    //Create an open file dialog box, so the user can select a .hex file.
    newFileName =
        QFileDialog::getOpenFileName(this, "Open Hex File", "", "Hex Files (*.hex *.ehx)");

    if(!newFileName.isEmpty())
    {
        UsbHid->ExchangeMessage(HID_PnP::FIRMWARE);

        FirmwareUpdate firmwareUpdate(newFileName, this);
        firmwareUpdate.exec();
    }
}

void ZeRefractorController::on_pushButton_addPosition_clicked()
{
    int new_row = ui->tableWidget_Positons->rowCount() + 1;
    ui->tableWidget_Positons->setRowCount(new_row);
    ui->tableWidget_Positons->update();

    QTableWidgetItem *newItem = new QTableWidgetItem(tr("New position"));
    ui->tableWidget_Positons->setItem(new_row - 1, 0, newItem);
    ui->tableWidget_Positons->editItem(newItem);

    newItem = new QTableWidgetItem(QString("%1").arg(Position));
    ui->tableWidget_Positons->setItem(new_row - 1, 1, newItem);

    QPushButton *newGoPushButton = new QPushButton(tr("Go"),this);
    positionsSignalMapper->setMapping(newGoPushButton, new_row - 1);
    connect(newGoPushButton, SIGNAL(clicked()), positionsSignalMapper, SLOT(map()));
    ui->tableWidget_Positons->setCellWidget(new_row - 1, 2, newGoPushButton);
}

void ZeRefractorController::on_pushButton_removePosition_clicked()
{
    if (ui->tableWidget_Positons->selectedItems().count() > 0)
    {
        int row = ui->tableWidget_Positons->selectedItems().at(0)->row();
        
        QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

        positionsSignalMapper->removeMappings(ui->tableWidget_Positons->cellWidget(row, 2));
        ui->tableWidget_Positons->removeRow(row);

        settings.remove("Positions");
        settings.beginWriteArray("Positions");
        for (row = 0 ; row < ui->tableWidget_Positons->rowCount() ; row++)
        {
            positionsSignalMapper->removeMappings(ui->tableWidget_Positons->cellWidget(row, 2));
            positionsSignalMapper->setMapping(ui->tableWidget_Positons->cellWidget(row, 2), row);
            
            settings.setArrayIndex(row);
            settings.setValue("Name", ui->tableWidget_Positons->item(row, 0)->text());
            settings.setValue("Position", ui->tableWidget_Positons->item(row, 1)->text());
        }
        settings.endArray();
    }
}

void ZeRefractorController::on_GoPushButton_clicked(int row)
{
    int position = ui->tableWidget_Positons->item(row, 1)->text().toInt();
    if (!isMoving)
    {
        destPostion = position;
        UsbHid->ExchangeMessage(HID_PnP::FPA, position, &position);
        setIsMoving(true);
    }
}

void ZeRefractorController::on_tableWidget_Positons_cellChanged(int row, int column)
{
    if (PositionsPopulated)
    {
        QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

        settings.beginWriteArray("Positions");
        settings.setArrayIndex(row);
        switch (column)
        {
        case 0:
            settings.setValue("Name", ui->tableWidget_Positons->item(row, column)->text());
            break;
        case 1:
            settings.setValue("Position", ui->tableWidget_Positons->item(row, column)->text());
            break;
        default:
            break;
        }
        settings.endArray();
    }
}
