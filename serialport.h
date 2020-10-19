#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QWidget>
#include <QSerialPort>
#include <QTimer>
#include <QEventLoop>
#include <QSemaphore>
#include <QStack>
#include <QThread>
#include "mainwindow.h"
#include <QMutex>

#define NB_BYTES 5

class SerialPort : public QThread
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    //Méthode de routine permettant la lecture indépendante au port série

    void run();

    //Méthode constante permettant l'accés aux attributs

    bool checkOpenSerial() const;
    MainWindow::Settings settingsInfo() const;
    QString serialError() const;

    void settingUpdate(MainWindow::Settings settingsActu);

    void startCloseSerial();


public slots:

    void pushSerial(const QByteArray &data);
    void readingData();

signals:
    void errorEmit(QString error);
    void dataEmit(bool receptionChek, QByteArray data);

    void openSuccess(bool open);
    void closeSuccess(bool close);

private:

    //Méthode intervenant dans la routine pour permettre l'exécution du code

    void openSerial();
    void closeSerial();
    void writeData(const QByteArray &data);

    void handleError(QSerialPort::SerialPortError error);
    void initActionsConnections();

    // Attributs

    MainWindow::Settings *m_settingsPort = nullptr;
    QSerialPort *m_serial = nullptr;
    QTimer *m_waitTimerReponse = nullptr;

    int m_waitTimeout = 2000;
    int m_readDataTry = 0;
    QSemaphore *m_semSerialPort;

    QSemaphore *m_semVide;
    QStack<QByteArray> m_stack;

    bool m_serialOpen = false;
};

#endif // SERIALPORT_H
