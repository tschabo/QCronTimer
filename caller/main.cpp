#include <QCoreApplication>

#include <ccrontimer.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
printf("%u\n", sizeof(time_t));
printf("%i\n", sizeof(int));
time_t xxx = INT_MAX;
printf(ctime(&xxx));
    CCronTimer t;
    qRegisterMetaType<CCronCalculator>("CCronCalculator");
    QMetaObject::invokeMethod(&t, "start", Qt::QueuedConnection, Q_ARG(CCronCalculator, CCronCalculator("* * * * *")));
    QObject::connect(&t, &CCronTimer::timeout,
                     [](){printf("666\n");});
    return a.exec();
}
