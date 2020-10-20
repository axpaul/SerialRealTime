#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include <QDebug>
#include "mythread.h"
#include <QWidget>
#include "serialport.h"


QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }

QT_END_NAMESPACE

class SerialPort;

class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void writeData(const QByteArray &data);

    void doSetup(QThread &cThread);
    int doWork();

public slots :
    void openSerialPort();
    void closeSerialPort();
    void handleErrorShow(QString error);
    void reponseDecode(bool reponseCheck, QByteArray data);

    void sendMessageByWorker(QByteArray data);
    void StartWoker();

    void settingUpdate();
    void settingShow();

    void openSerialSuccess(bool Open);
    void closeSerialSucess(bool close);

private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);

    Ui::MainWindow *ui;
    QLabel *m_status = nullptr;
    SerialPort *m_serial = nullptr;
    MyThread *m_worker = nullptr;

    SettingsDialog *m_settings = nullptr;
    SerialPort::Settings *m_settingsInfo = nullptr;



};
#endif // MAINWINDOW_H
