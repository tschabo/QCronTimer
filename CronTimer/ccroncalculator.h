#ifndef CCRONCALCULATOR_H
#define CCRONCALCULATOR_H

#include "crontimer_global.h"

#include <string>
#include <vector>
#include <map>
#include <mutex>

typedef time_t duration_t;

enum ECronPart
{
    MINUTE_OF_HOUR = 0,
    HOUR_OF_DAY,
    DAY_OF_MONTH,
    MONTH_OF_YEAR,
    DAY_OF_WEEK
};

struct SMinMax
{
    int min;
    int max;
};

class CRONTIMERSHARED_EXPORT CCronCalculator
{
public:
    CCronCalculator();
    CCronCalculator(const std::string &cronspec);
    CCronCalculator(const CCronCalculator &obj);
    ~CCronCalculator();

    bool isValid() const;
    std::string getCronspec() const;
    time_t nextExecution(bool bUTC = false);
    duration_t nextExecutionDiff(const time_t t, const bool bUTC = false);
    bool setCronspec(const std::string &cronspec);
    static tm toTm(const time_t &t, bool bUTC = false);

private:
    bool c_isValid = false;
    std::string c_cronspec;
    std::map<ECronPart, SMinMax> c_minMaxMap;
    std::map<ECronPart, std::vector<int>> c_partsMap;
    std::vector<std::string> split(const std::string &s, const char delimiter);
    bool initPart(const std::pair<ECronPart, SMinMax> &part, const std::string &crontabPartSpec);
    int getNextTo(const ECronPart part, const int val, bool *pOverflow = nullptr);
    int toInt(const std::string &s, bool *pOk = nullptr);
};

#endif // CCRONCALCULATOR_H
