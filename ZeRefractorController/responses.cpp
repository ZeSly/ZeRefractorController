#include "zerefractorcontroller.h"
#include "ui_zerefractorcontroller.h"

void ZeRefractorController::CreateResponseFuncList(void)
{
    ResponseFunc res;

    res.Response = "fss=";
    res.Func = &ZeRefractorController::Response_StepperMode;
    ResponseFuncList << res;

    res.Response = "fsa=";
    res.Func = &ZeRefractorController::Response_StepDelayAuto;
    ResponseFuncList << res;

    res.Response = "fsm=";
    res.Func = &ZeRefractorController::Response_StepDelayManual;
    ResponseFuncList << res;

    res.Response = "fsi=";
    res.Func = &ZeRefractorController::Response_MinimumPosition;
    ResponseFuncList << res;

    res.Response = "fso=";
    res.Func = &ZeRefractorController::Response_MaximumPosition;
    ResponseFuncList << res;

    res.Response = "fsp=";
    res.Func = &ZeRefractorController::Response_InitialPosition;
    ResponseFuncList << res;

    res.Response = "fsb=";
    res.Func = &ZeRefractorController::Response_Backlash;
    ResponseFuncList << res;

    res.Response = "ftc=";
    res.Func = &ZeRefractorController::Response_TemperarueCoeff;
    ResponseFuncList << res;

    res.Response = "fva=";
    res.Func = &ZeRefractorController::Response_fva;
    ResponseFuncList << res;

    res.Response = "v";
    res.Func = &ZeRefractorController::Response_version;
    ResponseFuncList << res;

    res.Response = "fsc";
    res.Func = &ZeRefractorController::Response_ConfigByte;
    ResponseFuncList << res;
}

void ZeRefractorController::Response_fva(char *response)
{
    double power_alarm = strtod(response + 4, NULL) / 10;
    ui->lineEdit_LowPowerAlarm->setText(QString("%1").arg((power_alarm)));
}

void ZeRefractorController::Response_version(char *response)
{
    ui->label_ver->setText(QString("Firmware %1").arg(response));
}

void ZeRefractorController::Response_StepperMode(char *response)
{
    int mode = atoi(response + 4);
    ui->comboBox_stepper_mode->setCurrentIndex(mode);
}

void ZeRefractorController::Response_StepDelayAuto(char *response)
{
    QString step = response + 4;
    ui->lineEdit_StepModeAuto->setText(step);
}

void ZeRefractorController::Response_StepDelayManual(char *response)
{
    QString step = response + 4;
    ui->lineEdit_StepModeManual->setText(step);
}

void ZeRefractorController::Response_MinimumPosition(char *response)
{
    QString step = response + 4;
    ui->lineEdit_MinPosition->setText(step);
}

void ZeRefractorController::Response_MaximumPosition(char *response)
{
    QString step = response + 4;
    ui->lineEdit_MaxPosition->setText(step);
}

void ZeRefractorController::Response_InitialPosition(char *response)
{
    QString step = response + 4;
    ui->lineEdit_InitialPosition->setText(step);
}

void ZeRefractorController::Response_Backlash(char *response)
{
    QString step = response + 4;
    ui->lineEdit_Backlash->setText(step);
}

void ZeRefractorController::Response_TemperarueCoeff(char *response)
{
    QString step = response + 4;
    ui->lineEdit_TemperatureCoeff->setText(step);
}

void ZeRefractorController::Response_ConfigByte(char *response)
{
    int ConfigByte = atoi(response + 4);

    ui->checkBox_SavePositionPowerfail->setChecked((ConfigByte & 1) != 0 ? true : false);
    ui->checkBox_InvertMotor->setChecked((ConfigByte & 2) != 0 ? true : false);
    ui->checkBox_PositiveDeltaTempMoveIn->setChecked((ConfigByte & 4) != 0 ? true : false);
    ui->checkBox_KeepPowerMotoStopped->setChecked((ConfigByte & 8) != 0 ? true : false);
}
