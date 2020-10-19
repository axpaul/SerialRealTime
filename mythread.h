#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QWidget>
#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread();

    void run();
    bool stop = false;

signals:
    void sendCommande(QByteArray);

private:
    QByteArray message1, message2;
};

#endif // MYTHREAD_H
