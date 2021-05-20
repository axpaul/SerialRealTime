#include "serialport.h"

Q_DECLARE_METATYPE(QSerialPort::SerialPortError);
Q_DECLARE_METATYPE(SerialPort::Settings);

SerialPort::SerialPort() {
}

SerialPort::~SerialPort() {
    delete m_serial;
    delete m_settingsPort;
    delete m_semSerialPort;
    delete m_semStack;
}

bool SerialPort::openSerial()
{
    m_serial->setPortName(m_settingsPort->name);
    m_serial->setBaudRate(m_settingsPort->baudRate);
    m_serial->setDataBits(m_settingsPort->dataBits);
    m_serial->setParity(m_settingsPort->parity);
    m_serial->setStopBits(m_settingsPort->stopBits);
    m_serial->setFlowControl(m_settingsPort->flowControl);

    m_serial->setReadBufferSize(0);

    if (m_serial->open(QIODevice::ReadWrite)) {
        m_serial->clear(QSerialPort::AllDirections);
        emit SerialPort::serialOpenned(this->settingsInfo());
        return true;
    }
    else
        return false;
}

void SerialPort::closeSerial()
{
    if (m_serial->isOpen()) {
        m_serial->clear(QSerialPort::AllDirections);
        m_serial->close();
        clearStack();
        emit SerialPort::serialClosed();
    }
}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] handleError";
    if (error != QSerialPort::NoError) {
        m_serialRun = false;
        qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] emit errorEmit";
        emit SerialPort::errorEmit(m_serial->errorString());
    }
}

QString SerialPort::serialError() const
{
    return m_serial->errorString();
}

bool SerialPort::checkOpenSerial() const
{
    return m_serial->isOpen();
}

SerialPort::Settings SerialPort::settingsInfo() const
{
    return *m_settingsPort;
}

void SerialPort::settingUpdate(SerialPort::Settings settingPort)
{
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] settingUpdate";
    *m_settingsPort = settingPort;
}

void SerialPort::writeData(const QByteArray data)
{
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] cmd sent : " << hex << data.toHex();
    m_serial->clear(QSerialPort::AllDirections);
    m_serial->write(data);
    m_serial->waitForBytesWritten();
}

void SerialPort::pushStack(QByteArray cmd) {
    mut.lock();
    m_stack.push(cmd);
    m_semStack->release(1);
    mut.unlock();
}

void SerialPort::clearStack() {
    if(!m_stack.isEmpty())
        m_stack.clear();
    while(m_semStack->tryAcquire(1))
        ;
    m_retry = 0;
}

void SerialPort::setSerialRun(bool onoff) {
    m_serialRun = onoff;
}

void SerialPort::run() {

    m_serial = new QSerialPort;
    m_settingsPort = new Settings;

    m_semSerialPort = new QSemaphore(1);
    m_semStack = new QSemaphore(0);

    m_serialRun = false;
    m_retry = 0;

    // Register types for connect
    qRegisterMetaType<QSerialPort::SerialPortError>();
    qRegisterMetaType<SerialPort::Settings>();

    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPort::handleError);

    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] " << QThread::currentThread();
    while(1) {
        if(m_serialRun) {
            if(this->openSerial()) {
                qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] Serial oppened";
            }
            else {
                qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][SERIAL] Serial failed";
                m_serialRun = false;
            }
            while(m_serialRun) {
                if(m_semStack->tryAcquire(1)) {
                    mut.lock();
                    QByteArray tmp = m_stack.pop();
                    mut.unlock();
                    writeData(tmp);
                }
                else
                    QThread::msleep(10);
            }
            this->closeSerial();
        }
        QThread::msleep(10);
    }
}
