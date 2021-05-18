#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    console = ui->console_Window;

    m_serial = new SerialPort;
    m_serialRun = false;

    m_semSendCmd = new QSemaphore(1);

     m_serial->start();

     initActionsConnectionsPrio();

     m_settings = new SettingsDialog;
     setSerialSettings();

     m_connection = new QString;
     m_versionSW = new QString;

     ui->statusbar->addWidget(m_status);

    initActionsConnections();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][MAINWINDOW] " << QThread::currentThread();
}

MainWindow::~MainWindow(){

   // closeSerialPort();

    delete m_connection;
    delete m_versionSW;
    delete ui;
}

/* Connect function sender/signal */

void MainWindow::initActionsConnectionsPrio(){

    connect(this, SIGNAL(setSerialSettingsSig(SerialPort::Settings)), m_serial, SLOT(settingUpdate(SerialPort::Settings)));
    connect(m_serial, SIGNAL(errorEmit(QString)), this, SLOT(handleErrorShow(QString)));

}

void MainWindow::initActionsConnections(){
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::showSetting); // set setting serial
    connect(m_settings, &SettingsDialog::applyParameter, this, &MainWindow::setSerialSettings);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);

    connect(m_serial, SIGNAL(serialOpenned(SerialPort::Settings)), this, SLOT(opennedSerial(SerialPort::Settings)));
    connect(m_serial, SIGNAL(serialClosed()), this, SLOT(closedSerial()));

    connect(ui->button_Send, &QPushButton::clicked, this, &MainWindow::cmdToSend);

    connect(this, SIGNAL(sendCommandSerial(QByteArray)), m_serial, SLOT(pushStack(QByteArray)));
    connect(ui->actionClearConsole, &QAction::triggered, ui->console_Window, &QPlainTextEdit::clear);

}

/* MainWindow Information */

void MainWindow::about(){
    QString textAbout;
    textAbout.asprintf("Serial Interface v2/v3 Interface\nVersion : %.1f", VERSION_SERIAL);
    QMessageBox::about(this,"About", textAbout);
}

void MainWindow::showStatusMessage(const QString &stringConnection, const QString &versionSW){
    QString message;

    if (stringConnection != "" && stringConnection != *m_connection)
    {
        *m_connection = stringConnection;
    }

    if (versionSW != "" && versionSW != *m_versionSW)
    {
        *m_versionSW = versionSW;
    }

    message = QString("%1 | %2").arg(*m_connection).arg(*m_versionSW);

   // m_status->setText(message);
}

/* Functions settings systems */

void MainWindow::settingShow(){
    m_settings->show();
}

void MainWindow::setSerialSettings() {
    emit setSerialSettingsSig(m_settings->settings());
}

SerialPort::Settings MainWindow::getSerialInfo() {
    return m_serial->settingsInfo();
}

/* Function open/close serial */

void MainWindow::opennedSerial(SerialPort::Settings p) {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][MAINWINDOW] Serial openned";
    m_serialRun = true;

    showStatusMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl), "");

}

void MainWindow::closedSerial() {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Serial closed";
    m_serialRun = false;

    m_serial->clearStack();

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);

    showStatusMessage(QString("Disconnected"), " ");
}


void MainWindow::openSerialPort() {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Send Serial open";
    m_serial->setSerialRun(true);

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::closeSerialPort() {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Send Serial close";
    m_serial->setSerialRun(false);

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
}

/* Error function */

void MainWindow::handleErrorShow(QString error){
   QMessageBox::critical(this, QString("Error"), error);
}


QString MainWindow::getSerialError() {
    return m_serial->serialError();
}

/* Write function */


void MainWindow::cmdToSend()
{

   QByteArray dataToSend((ui->lineEdit_cmd->text()).toLocal8Bit());

   dataToSend.append("\r");

   qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Send data : " << Qt::hex << dataToSend.toHex();

   QByteArray dataToShow("Send -> ");

   ui->lineEdit_cmd->clear();
   console->putData(dataToShow.append(dataToSend));
   //console->putData("\n");

   emit sendCommandSerial(dataToSend);
}


