#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include "serialport.h"

#define VERSION_LASER_PORFILER 2.0f

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
    void initActionsConnections();


private:
        Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
