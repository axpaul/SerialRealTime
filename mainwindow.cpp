#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_settings = new SettingsDialog;
    emit setSerialSettingsSig(m_settings->settings());

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

void MainWindow::initActionsConnections(){
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);

    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::showSetting); // set setting serial
    connect(m_settings, SIGNAL(applyParameter()), this, SLOT(setSerialSettings()));
    //connect(ui->a, &QAction::triggered, ui->console, &QPlainTextEdit::clear);
}

void MainWindow::about(){
    QString textAbout;
    textAbout.sprintf("Serial Interface v2/v3 Interface\nVersion : %.1f", VERSION_SERIAL);
    QMessageBox::about(this,"About", textAbout);
}

void MainWindow::settingShow(){
    m_settings->show();
}

void MainWindow::setSerialSettings() {
    emit setSerialSettingsSig(m_settings->settings());
}

void MainWindow::openSerialPortInfo(SerialPort::Settings p){
    showStatusMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl), "");

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
}

void MainWindow::closeSerialPortInfo(){
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);

    showStatusMessage(QString("Disconnected"), " ");
}

void MainWindow::handleErrorShow(QString error){
   QMessageBox::critical(this, QString("Error"), error);
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

    m_status->setText(message);
}
