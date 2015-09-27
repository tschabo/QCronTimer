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

    static void singleShot(const CCronCalculator& cc);

signals:
    void timeout();

public slots:
    void start(const CCronCalculator& cc);
    void stop();

private slots:
    void internTimedOut();

private:
    void privateStart();
    bool c_singleShot = false;
    bool c_overflow = false;
    bool c_notAccurate = false;
    CCronCalculator c_cronCalc;
    time_t c_nextExec = 0;
};

#endif // CCRONTIMER_H
