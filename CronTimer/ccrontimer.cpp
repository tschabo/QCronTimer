#include "ccrontimer.h"
#include <limits.h>
#include <assert.h>
#ifdef CRONTIMER_DEBUG
#include <stdio.h>
#endif

CCronTimer::CCronTimer(QObject *parent) :
    QObject(parent)
{
}

void CCronTimer::setSingleShot(bool singleShot)
{
    c_singleShot = singleShot;
}

void CCronTimer::singleShot(const CCronCalculator &cc)
{

}

void CCronTimer::start(const CCronCalculator &cc)
{
    c_cronCalc.setCronspec(cc.getCronspec());
    c_nextExec = c_cronCalc.nextExecution();
    privateStart();
}

void CCronTimer::stop()
{
}

void CCronTimer::internTimedOut()
{
    if(!c_overflow)
    {

        if(!c_notAccurate)
        {
#ifdef CRONTIMER_DEBUG
            printf("timeout() signaled !\n");
#endif
            emit timeout();
        }

        if(c_singleShot)
            return;

        if(c_nextExec == c_cronCalc.nextExecution())
        {
#ifdef CRONTIMER_DEBUG
            printf("timer was not accurate !\n");
#endif
            c_notAccurate = true;
            QTimer::singleShot(500, this, SLOT(internTimedOut()));
        }
        else
        {
            c_notAccurate = false;
            start(c_cronCalc);
#ifdef CRONTIMER_DEBUG
            printf("restarted timer (not singleshot) !\n");
#endif
            return;
        }
    }
    else
    {
#ifdef CRONTIMER_DEBUG
        printf("restart after overflow !\n");
#endif
        privateStart();
        return;
    }

    emit timeout();

    if(!c_singleShot)
    {
        time_t next = c_cronCalc.nextExecution();
#ifdef CRONTIMER_DEBUG
        printf("%i ... %i \n", c_nextExec, next);
#endif
        if(c_nextExec == next)
        {
            c_notAccurate = true;
            int nextExecMs = (time(nullptr) - c_nextExec) * 1000;
#ifdef CRONTIMER_DEBUG
            printf("%i\n",  nextExecMs);
#endif
            if (nextExecMs > 0)

            QTimer::singleShot(nextExecMs, this, SLOT(internTimedOut()));
        }
        start(c_cronCalc);
    }
}

void CCronTimer::privateStart()
{
    time_t diff = c_cronCalc.nextExecutionDiff(time(nullptr));

    assert(diff >= 0);

    c_overflow = (diff >= INT_MAX / 1000) ? true : false;

    if(c_overflow)
        QTimer::singleShot(INT_MAX, this, SLOT(internTimedOut()));
    else
        QTimer::singleShot(diff * 1000, this, SLOT(internTimedOut()));
}
