#ifndef SERIALPORT_H
#define SERIALPORT_H

#define ERRORSERIALBYTE 5

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QSemaphore>
#include <QStack>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <QDateTime>

class SettingsDialog;

class SerialPort : public QThread
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };

    SerialPort();
    ~SerialPort();

    bool openSerial();
    void handleError(QSerialPort::SerialPortError error);

    void closeSerial();
    void writeData(const QByteArray data);

    bool checkOpenSerial() const;
    Settings settingsInfo() const;
    QString serialError() const;
    void setSerialRun(bool onoff);
    void run() override;
    void clearStack();

public slots:
    void settingUpdate(SerialPort::Settings settingPort);
    void readingData();
    void pushStack(QByteArray cmd, int byte);

signals:
    void errorEmit(QString);
    void dataEmit(bool receptionChek, QByteArray data);
    void serialOpenned(SerialPort::Settings p);
    void serialClosed();

private:
    Settings *m_settingsPort = nullptr;
    QSerialPort *m_serial = nullptr;
    QTimer *m_waitTimerReponse = nullptr;
    bool m_serialRun;

    int m_waitTimeout = 100000;
    int m_readDataTry = 0;

    QSemaphore *m_semSerialPort;
    QSemaphore *m_semStack;
    QMutex mut;
    uint8_t m_retry;

    QStack<QByteArray> m_stackCmd;
    QStack<int> m_stackByte;

    int m_byte;


};

#endif // SERIALPORT_H
