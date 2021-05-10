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


public slots :
    void openSerialPort(SerialPort::Settings p);
    void closeSerialPort();
    void handleErrorShow(QString error);
    void settingShow();

private slots :
    void about();

signals:
    void setSerialSettingsSig(SerialPort::Settings);

private:
    void initActionsConnections();

     void showStatusMessage(const QString &stringConnection, const QString &versionSW);

     Ui::MainWindow *ui;
     QLabel *m_status = nullptr;
     QString *m_connection;
     QString *m_versionSW;
     SettingsDialog *m_settings = nullptr;

};
#endif // MAINWINDOW_H
