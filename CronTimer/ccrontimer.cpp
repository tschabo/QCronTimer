#include "ccrontimer.h"
#include <limits.h>
#include <assert.h>
#include <QDateTime>
#include <QTimerEvent>
#include <stdio.h>

#include <QDebug>


CCronTimer::CCronTimer(QObject *parent) :
    QObject(parent)
{
}

void CCronTimer::setSingleShot(bool singleShot)
{
    c_singleShot = singleShot;
}

void CCronTimer::start(const CCronCalculator &cc)
{
    c_cronCalc.setCronspec(cc.getCronspec());
    std::lock_guard<std::mutex> lock(c_mtxNextExec);
    c_nextExec = c_cronCalc.nextExecution();
    c_timerId = startTimer(calcDiffTime());
    assert(c_timerId);
}

void CCronTimer::stop()
{
    std::lock_guard<std::mutex> lock(c_mtxNextExec);
    c_nextExec = -1;
    killTimer(c_timerId);
}

void CCronTimer::timerEvent(QTimerEvent *pEvent)
{
    if(pEvent->timerId() != c_timerId)
        return;

    qDebug() << "intern timeout";
    std::lock_guard<std::mutex> lock(c_mtxNextExec);
    killTimer(c_timerId);
    c_timerId = 0;
    if(c_nextExec < time(nullptr))
    {
        c_timerId = startTimer(calcDiffTime());
        assert(c_timerId);
    }
    else
    {
        emit timeout();
        if(!c_singleShot)
        {
            c_nextExec = c_cronCalc.nextExecution();
            c_timerId = startTimer(calcDiffTime());
        }
    }
}

time_t CCronTimer::calcDiffTime()
{
    time_t t = QDateTime::currentDateTime().msecsTo(QDateTime::fromTime_t(0).addSecs(c_nextExec));
    if(t < 0)
        return 0;
    if(t > INT_MAX)
        return INT_MAX;
    return t;
}
