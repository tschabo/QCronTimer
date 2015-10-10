#include <QCoreApplication>

#include <ccrontimer.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CCronTimer t;
    qRegisterMetaType<CCronCalculator>("CCronCalculator");
    QMetaObject::invokeMethod(&t, "start", Qt::QueuedConnection, Q_ARG(CCronCalculator, CCronCalculator("* * * * *")));
    QObject::connect(&t, &CCronTimer::timeout,
                     [](){time_t x = time(nullptr); printf("%s\n", ctime(&x));});
    return a.exec();
}
