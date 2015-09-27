#include <QCoreApplication>

#include <ccrontimer.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CCronTimer t;
    t.start(CCronCalculator("* * * * *"));

    return a.exec();
}
