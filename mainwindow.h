#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include "serialport.h"
#include "settingsdialog.h"

#define VERSION_SERIAL 1.0f

QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }

QT_END_NAMESPACE

class SerialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    SerialPort::Settings getSerialInfo();
    QString getSerialError();


public slots :
    /*void openSerialPortInfo(SerialPort::Settings p);
    void closeSerialPortInfo();*/

    void handleErrorShow(QString error);
    void settingShow();
    void setSerialSettings();

    void cmdToSend(uint8_t cmdNumber, uint16_t data);
    void cmdToSend(uint8_t type, uint8_t add, uint8_t rw, uint16_t data);
    void responseDecode(bool responseCheck, QByteArray data);
    void opennedSerial(SerialPort::Settings p);
    void closedSerial();
    void openSerialPort();
    void closeSerialPort();

private slots :
    void about();

signals:
    void setSerialSettingsSig(SerialPort::Settings);
    void serialOppened(SerialPort::Settings p);
    void serialClosed();
    void sendCommandSerial(QByteArray data);

private:
    void initActionsConnections();

     void showStatusMessage(const QString &stringConnection, const QString &versionSW);

     Ui::MainWindow *ui;

     QObject *m_parent;

     QLabel *m_status = nullptr;
     QString *m_connection;
     QString *m_versionSW;

     SettingsDialog *m_settings = nullptr;

     SerialPort *m_serialLoac;
     QSemaphore *m_semSendCmd;

     bool m_serialRun;
     QThread *m_serialThread;

     int *m_numberCommandeSend;

};
#endif // MAINWINDOW_H
