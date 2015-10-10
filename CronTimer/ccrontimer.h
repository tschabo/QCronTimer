#ifndef CCRONTIMER_H
#define CCRONTIMER_H

#include "crontimer_global.h"

#include <QObject>

#include "ccroncalculator.h"
#include <QTimer>


class CRONTIMERSHARED_EXPORT CCronTimer : public QObject
{
    Q_OBJECT
public:
    explicit CCronTimer(QObject *parent = 0);

    void setSingleShot(bool singleShot);

signals:
    void timeout();

public slots:
    void start(const CCronCalculator& cc);
    void stop();

private:
    CCronCalculator c_cronCalc;
    bool c_singleShot= false;
    int c_timerId = 0;
    time_t c_nextExec = -1;
    std::mutex c_mtxNextExec;

    void timerEvent(QTimerEvent *pEvent) override;

    time_t calcDiffTime();
};

#endif // CCRONTIMER_H
