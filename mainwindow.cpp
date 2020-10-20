#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialport.h"
#include "settingsdialog.h"
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_status(new QLabel), m_settingsInfo(new SerialPort::Settings)
{
    ui->setupUi(this);

    qDebug() << "Window :" << QThread::currentThread() << Qt::endl;

    m_serial = new SerialPort();
    m_worker = new MyThread;

    m_settings = new SettingsDialog;

    *m_settingsInfo = m_settings->settings();

    delete m_settings;
    m_settings = nullptr;


    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->button_Start->setEnabled(false);

    ui->statusbar->addWidget(m_status);

    initActionsConnections();
}

MainWindow::~MainWindow()
{
    /*m_serial->quit();
    m_worker->quit();
    m_serial->terminate();
    m_worker->terminate();*/

    delete m_settings;
    delete m_settingsInfo;
    delete m_status;
    delete m_worker;
    delete m_serial;
    delete ui;
}

void MainWindow::initActionsConnections()
{
    connect(m_serial, &SerialPort::errorEmit, this, &MainWindow::handleErrorShow);
    connect(m_serial, &SerialPort::dataEmit, this, &MainWindow::reponseDecode);
    connect(m_serial, &SerialPort::openSuccess, this, &MainWindow::openSerialSuccess);
    connect(m_serial, &SerialPort::closeSuccess, this, &MainWindow::closeSerialSucess);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(ui->actionConfigure, &QAction::triggered, this, &MainWindow::settingShow);

    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

    connect(ui->button_Start, &QPushButton::clicked, this, &MainWindow::StartWoker);

    connect(m_worker, &MyThread::sendCommande, this, &MainWindow::sendMessageByWorker);

}

void MainWindow::openSerialPort()
{
   m_serial->settingUpdate(*m_settingsInfo);
   m_serial->start();
}

void MainWindow::openSerialSuccess(bool Open)
{
    if (Open == true)
    {
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        ui->button_Start->setEnabled(true);


        //m_serial->DoSetupSerialThread(*m_threadSerial);

        showStatusMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(m_settingsInfo->name).arg(m_settingsInfo->stringBaudRate).arg(m_settingsInfo->stringDataBits)
                          .arg(m_settingsInfo->stringParity).arg(m_settingsInfo->stringStopBits).arg(m_settingsInfo->stringFlowControl));
    }

    else
    {
        QMessageBox::critical(this, QString("Error"),m_serial->serialError());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->checkOpenSerial())
    {
        m_serial->startCloseSerial();
    }

}

void MainWindow::closeSerialSucess(bool close)
{
    if (close == true)
    {
        ui->actionConfigure->setEnabled(true);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        ui->button_Start->setEnabled(false);

        showStatusMessage(QString("Disconnected"));
    }
}

void MainWindow::handleErrorShow(QString error)
{
   QMessageBox::critical(this, QString("Critical Error"), error);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::StartWoker()
{
    m_worker->start();
}

void MainWindow::reponseDecode(bool reponseCheck, QByteArray data)
{
    QString stringData;
    QByteArray cmd;
    QByteArray valueCmd;
    bool check;
    double value;
    uint8_t cmdNumber;

    if (reponseCheck == false)
    {
        QMessageBox::critical(this,"Error","Error on reception -> No response");
    }
    else
    {
        stringData = QString(data.toHex());

                valueCmd = data.right(3);
                valueCmd = valueCmd.left(2);
                value = double((QString(valueCmd.toHex())).toUInt(&check, 16));

                cmd = data.left(2);
                cmd = cmd.right(1);

                cmdNumber = (QString(cmd.toHex())).toUInt(&check, 16);

                uint8_t type = (cmdNumber & 0b11100000) >> 5;
                uint8_t add = (cmdNumber & 0b00011100) >> 2;
                uint8_t rw = (cmdNumber & 0b00000010) >> 1;
                //uint8_t ack = (cmdNumber & 0b00000001);

                uint8_t CommandCorrected = ( (type << 5) | (add << 1) | rw);

                if (CommandCorrected == 0xE4)
                {
                    if (value == 0x0000)
                    {
                        ui->console->setText("Turn on\n");
                    }
                    else if (value == 0x0001)
                    {
                        ui->console->setText("Turn off\n");
                    }
                }
    }

}

void MainWindow::sendMessageByWorker(QByteArray data)
{
    qDebug() << "Message à envoyer : " << data << Qt::endl;
    m_serial->pushSerial(data);
}

void MainWindow::settingUpdate()
{
    *m_settingsInfo = m_settings->settings();
}

void MainWindow::settingShow()
{
    m_settings = new SettingsDialog;

    connect(m_settings, &SettingsDialog::applyParameter, this, &MainWindow::settingUpdate);

    m_settings->show();


}

