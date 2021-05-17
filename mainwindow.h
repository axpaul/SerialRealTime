#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QLabel>
#include <QtDebug>
#include <QWidget>

#include "console.h"
#include "serialport.h"
#include "settingsdialog.h"

#define VERSION_SERIAL 1.0f

QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }

QT_END_NAMESPACE

class SerialPort;
class Console;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    SerialPort::Settings getSerialInfo();
    QString getSerialError();


public slots :

    void handleErrorShow(QString error);
    void settingShow();
    void setSerialSettings();

    /*void cmdToSend(uint8_t cmdNumber, uint16_t data);
    void cmdToSend(uint8_t type, uint8_t add, uint8_t rw, uint16_t data);*/

    void cmdToSend();

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
    void sendCommandSerial(QByteArray data, int byte);

private:
    void initActionsConnections();
    void initActionsConnectionsPrio();

     void showStatusMessage(const QString &stringConnection, const QString &versionSW);

     Ui::MainWindow *ui;

     QObject *m_parent;

     QLabel *m_status = nullptr;
     QString *m_connection;
     QString *m_versionSW;

     SettingsDialog *m_settings = nullptr;

     SerialPort *m_serial;
     QSemaphore *m_semSendCmd;
     bool m_serialRun;

     int *m_numberCommandeSend;

     Console *console;

};
#endif // MAINWINDOW_H
