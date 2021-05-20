#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QSemaphore>
#include <QStack>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <QDateTime>

class Motor : public QThread
{
    Q_OBJECT

public:
    Motor();
    ~Motor();
    void run();


public slots:


signals:

private:

};

#endif // MOTOR_H
