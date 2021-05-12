#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialLoac = new SerialPort;
    m_serialRun = false;

    m_semSendCmd = new QSemaphore(1);

     m_serialLoac->start();

     initActionsConnectionsPrio();

     m_settings = new SettingsDialog;
     setSerialSettings();

     m_connection = new QString;
     m_versionSW = new QString;

     ui->statusbar->addWidget(m_status);

    initActionsConnections();


    qDebug() << "[MAINWINDOW] " << QThread::currentThread();
}

MainWindow::~MainWindow(){

   // closeSerialPort();

    delete m_connection;
    delete m_versionSW;
    delete ui;
}

/* Connect function sender/signal */

void MainWindow::initActionsConnectionsPrio(){

    connect(this, SIGNAL(setSerialSettingsSig(SerialPort::Settings)), m_serialLoac, SLOT(settingUpdate(SerialPort::Settings)));
    connect(m_serialLoac, SIGNAL(errorEmit(QString)), this, SLOT(handleErrorShow(QString)));

}

void MainWindow::initActionsConnections(){
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::showSetting); // set setting serial
    connect(m_settings, SIGNAL(applyParameter()), this, SLOT(setSerialSettings()));

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);

    connect(m_serialLoac, SIGNAL(serialOpenned(SerialPort::Settings)), this, SLOT(opennedSerial(SerialPort::Settings)));
    connect(m_serialLoac, SIGNAL(serialClosed()), this, SLOT(closedSerial()));


    connect(this, SIGNAL(sendCommandSerial(QByteArray)), m_serialLoac, SLOT(pushStack(QByteArray)));
    connect(m_serialLoac, SIGNAL(dataEmit(bool, QByteArray)), this, SLOT(responseDecode(bool, QByteArray)));

    //connect(ui->a, &QAction::triggered, ui->console, &QPlainTextEdit::clear);
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
    return m_serialLoac->settingsInfo();
}

/* Function open/close serial */

void MainWindow::opennedSerial(SerialPort::Settings p) {
    qDebug() << "[MAINWINDOW] Serial openned";
    m_serialRun = true;

    showStatusMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl), "");

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::closedSerial() {
    qDebug() << "[MAINWINDOW] Serial closed";
    m_serialRun = false;

    m_serialLoac->clearStack();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    showStatusMessage(QString("Disconnected"), " ");
}


void MainWindow::openSerialPort() {
    qDebug() << "[MAINWINDOW] Send Serial open";
    m_serialLoac->setSerialRun(true);
}

void MainWindow::closeSerialPort() {
    qDebug() << "[MAINWINDOW] Send Serial close";
    m_serialLoac->setSerialRun(false);
}

/* Error function */

void MainWindow::handleErrorShow(QString error){
   QMessageBox::critical(this, QString("Error"), error);
}


QString MainWindow::getSerialError() {
    return m_serialLoac->serialError();
}

/* Write/read function */

void MainWindow::responseDecode(bool responseCheck, QByteArray data)
{
    /*QString stringData;
    QByteArray cmd;
    QByteArray valueCmd;
    bool check;
    double value;
    uint8_t cmdNumber;

    emit consoleDebugSignal(responseCheck, data);

    if (responseCheck == false)
    {
        qDebug() << "[LOAC] Error response check" << endl;
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
        uint8_t ack = (cmdNumber & 0b00000001);

        uint8_t CommandReceived = ( (type << 5) | (add << 1) | rw);*/

       /* qDebug() << "Data :" << value << "Cmd :" << hex << cmdNumber << "\n";
        qDebug() << "type :" << bin << type << "\t add :"  << bin << add << "\t rw :" << bin << rw << "\t ack :" << bin << ack << "\n";
        qDebug() << "Cmd Good :" << hex << CommandCorrected << "\n";
        qDebug() << "String :" << stringData << "\n";*/

        /*if(ack != 1)
        {
            commandeStore(CommandReceived, value);
            emit modifCommandePanel(CommandReceived, value);

            if (m_scenario == true)
            {
                emit scenarioDataReceived(CommandReceived, value);
                m_dataValueSceReceived = value;
                qDebug() << "[SCENARIO] Data value : " << m_dataValueSceReceived << "\n";
            }

        }
    }*/
}

void MainWindow::cmdToSend(uint8_t type, uint8_t add, uint8_t rw, uint16_t data) {
    QByteArray cmd;
    if(rw == 0) {
        cmd.resize(5);
        cmd[0] = 0x5b;
        cmd[1] = (type << 5) | (add << 1) | rw;
        cmd[2] = data >> 8;
        cmd[3] = data & 0xFF;
        cmd[4] = 0x5d;
    }
    else {
        cmd.resize(3);
        cmd[0] = 0x5b;
        cmd[1] = (type << 5) | (add << 1) | rw;
        cmd[2] = 0x5d;
    }
    emit MainWindow::sendCommandSerial(cmd);
}

void MainWindow::cmdToSend(uint8_t cmdNumber, uint16_t data) {
    QByteArray cmd;
    if((cmdNumber & 0x1) == 0) {
        cmd.resize(5);
        cmd[0] = 0x5b;
        cmd[1] = cmdNumber;
        cmd[2] = data >> 8;
        cmd[3] = data & 0xFF;
        cmd[4] = 0x5d;
    }
    else {
        cmd.resize(3);
        cmd[0] = 0x5b;
        cmd[1] = cmdNumber;
        cmd[2] = 0x5d;
    }
    emit MainWindow::sendCommandSerial(cmd);
}
