#include "mythread.h"
#include <QMutex>
#include <QDebug>

MyThread::MyThread()
{
    message1.resize(5);
    message1[0] = 0x5b;
    message1[1] = 0xe4;
    message1[2] = 0x00;
    message1[3] = 0x00;
    message1[4] = 0x5d;

    message2.resize(5);
    message2[0] = 0x5b;
    message2[1] = 0xe4;
    message2[2] = 0x00;
    message2[3] = 0x01;
    message2[4] = 0x5d;
}

void MyThread::run()
{
     qDebug() << "Worker :" << QThread::currentThread() << Qt::endl;

    for (int i = 0 ; i < 10 ; i++)
    {
        QMutex mutex;
        mutex.lock();
        if(this->stop) break;
        mutex.unlock();

        emit sendCommande(message1);
        QThread::msleep(300);
        emit sendCommande(message2);
        QThread::msleep(300);
    }
}

