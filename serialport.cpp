#include <QMessageBox>
#include <QLabel>
#include "serialport.h"
#include "settingsdialog.h"
#include <QDebug>

Q_DECLARE_METATYPE(QSerialPort::SerialPortError)
Q_DECLARE_METATYPE(SerialPort::Settings)

SerialPort::SerialPort(QObject *parent)
    : QThread(parent)
{
    m_waitTimerReponse = new QTimer;
    m_semSerialPort = new QSemaphore(1);
    m_semVide = new QSemaphore(0);
    m_settingsPort = new SerialPort::Settings;
}

SerialPort::~SerialPort()
{
    delete m_semSerialPort;
    delete m_settingsPort;
    delete m_serial;
    delete m_waitTimerReponse;
    delete m_semVide;
}

void SerialPort::openSerial()
{
    m_serial->setPortName(m_settingsPort->name);
    m_serial->setBaudRate(m_settingsPort->baudRate);
    m_serial->setDataBits(m_settingsPort->dataBits);
    m_serial->setParity(m_settingsPort->parity);
    m_serial->setStopBits(m_settingsPort->stopBits);
    m_serial->setFlowControl(m_settingsPort->flowControl);

    m_serial->setReadBufferSize(0);

    if (m_serial->open(QIODevice::ReadWrite))
    {
        m_serial->clear(QSerialPort::AllDirections);

        m_serialOpen = true;

        emit openSuccess(true);
    }

    else
    {
        m_serialOpen = false;

        emit openSuccess(false);
    }
}

void SerialPort::closeSerial()
{
    if (m_serial->isOpen())
    {
        m_serialOpen = false;

        m_serial->clear(QSerialPort::AllDirections);
        m_serial->close();

        emit closeSuccess(true);
    }
    else
    {
        emit closeSuccess(false);
    }

}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        closeSerial();
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

void SerialPort::writeData(const QByteArray &data)
{
    m_semSerialPort->acquire(1);
    qDebug() << "[SERIAL] cmd sent : " << Qt::hex << data.toHex();
    m_serial->clear(QSerialPort::AllDirections);
    m_serial->write(data);
    readingData();
    m_semSerialPort->release(1);
}

SerialPort::Settings SerialPort::settingsInfo() const
{
    return *m_settingsPort;
}

void SerialPort::readingData() {

    if(m_serial->waitForReadyRead(m_waitTimeout))
    {
          if(m_serial->bytesAvailable() >= 5)
          {
              QByteArray responseData = m_serial->read(5);
              qDebug() << "[SERIAL] Data received : " << Qt::hex << responseData.toHex();

              if (!responseData.isEmpty() && responseData.size() == 5)
              {
                  m_serial->clear(QSerialPort::AllDirections);
                  emit dataEmit(true, responseData);
              }
          }
          else
          {
              m_serial->clear(QSerialPort::AllDirections);
              emit dataEmit(false, "");
              qDebug() << "[SERIAL] Reception error !";
          }
      }

      else
      {
          m_serial->clear(QSerialPort::AllDirections);
          emit dataEmit(false, "");
          qDebug() << "[SERIAL] Timeout error !";
      }
}

void SerialPort::settingUpdate(SerialPort::Settings settingsActu)
{
   *m_settingsPort = settingsActu;
}

void SerialPort::run()
{
    qDebug() << "SerialPort :" << QThread::currentThread() << Qt::endl;

    m_serial = new QSerialPort();

    qRegisterMetaType<QSerialPort::SerialPortError>();
    qRegisterMetaType<SerialPort::Settings>();

    initActionsConnections();

    openSerial();

    while(m_serialOpen == true)
    {
        QMutex mutex;
        mutex.lock();
        m_semVide->acquire(1);

        if (m_serialOpen == true)
        {
            //qDebug() << "Serial listen : " << m_stack.pop() << Qt::endl;
            writeData(m_stack.pop());        
        }
        else
        {
            closeSerial();
        }

        mutex.unlock();
    }
}

void SerialPort::pushSerial(const QByteArray &data)
{
    QMutex mutex;
    mutex.lock();
    m_stack.push(data);
    m_semVide->release(1);
    mutex.unlock();
}

void SerialPort::initActionsConnections()
{
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
    connect(m_waitTimerReponse, &QTimer::timeout, this, &SerialPort::readingData);
}

void SerialPort::startCloseSerial()
{
    m_serialOpen = false;
    m_semVide->release(1);
}

