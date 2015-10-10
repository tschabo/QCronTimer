#include "ccroncalculator.h"

#include <algorithm>
#include <time.h>

#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
#define SAFEGMTIME(X,Y) gmtime_r(X,Y)
#define SAFELOCALTIME(X,Y) localtime_r(X,Y)
#elif _WIN32
#define SAFEGMTIME(X,Y) gmtime_s(Y,X)
#define SAFELOCALTIME(X,Y) localtime_s(Y,X)
#endif

CCronCalculator::CCronCalculator()
{
}

CCronCalculator::CCronCalculator(const std::string &cronspec)
{
    c_isValid = setCronspec(cronspec);
}

CCronCalculator::CCronCalculator(const CCronCalculator &obj)
{
    c_isValid = setCronspec(obj.getCronspec());
}

CCronCalculator::~CCronCalculator()
{

}

bool CCronCalculator::isValid() const
{
    return c_isValid;
}

std::string CCronCalculator::getCronspec() const
{
    return c_cronspec;
}

time_t CCronCalculator::nextExecution(bool bUTC)
{
    time_t next = time(nullptr);

    if(next == -1)
        return -1;

    tm converted = toTm(next, bUTC);

    next = next - converted.tm_sec; // reset seconds
    next = next + 60; // next minute

    while(true)
    {
        converted = toTm(next);
        if(converted.tm_mon != getNextTo(MONTH_OF_YEAR, converted.tm_mon))
        {
            next = next + (60 * 60 * 24); // add a day till month fits
            next = next - (converted.tm_hour * 60 * 60); // reset hours
            next = next - (converted.tm_min * 60); // reset minutes
            continue;
        }
        if(converted.tm_mday != getNextTo(DAY_OF_MONTH, converted.tm_mday))
        {
            next = next + (60 * 60 * 24); // add a day till day of month fits
            next = next - (converted.tm_hour * 60 * 60); // reset hours
            next = next - (converted.tm_min * 60); // reset minutes
            continue;
        }
        if(converted.tm_wday != getNextTo(DAY_OF_WEEK, converted.tm_wday))
        {
            next = next + (60 * 60 * 24); // add a day till day of week fits
            next = next - (converted.tm_hour * 60 * 60); // reset hours
            next = next - (converted.tm_min * 60); // reset minutes
            continue;
        }
        if(converted.tm_hour != getNextTo(HOUR_OF_DAY, converted.tm_hour))
        {
            next = next + (60 * 60); // add a hour till hour fits
            next = next - (converted.tm_min * 60); // reset minutes
            continue;
        }
        if(converted.tm_min != getNextTo(MINUTE_OF_HOUR, converted.tm_min))
        {
            next = next + (60); // add a minute till minutes fits
            continue;
        }
        break;
    }

    return next;
}

duration_t CCronCalculator::nextExecutionDiff(const time_t t, const bool bUTC)
{
    time_t next = nextExecution(bUTC);
    if(next == -1)
        return next;

    return next - t;
}

std::vector<std::string> CCronCalculator::split(const std::string &s, const char delimiter)
{
    std::vector<std::string> list;
    std::string current;

    for(const char c : s)
    {
        if(c == delimiter)
        {
            if(!current.empty())
                list.push_back(current);

            current.clear();
            continue;
        }
        current.push_back(c);
    }

    if(!current.empty())
        list.push_back(current);

    return list;
}

bool CCronCalculator::setCronspec(const std::string &cronspec)
{
    c_minMaxMap.clear();
    c_partsMap.clear();

    c_cronspec = cronspec;

    c_minMaxMap[MINUTE_OF_HOUR] = SMinMax{0, 59};
    c_minMaxMap[HOUR_OF_DAY] = SMinMax{0, 23};
    c_minMaxMap[DAY_OF_MONTH] = SMinMax{1, 31};
    c_minMaxMap[MONTH_OF_YEAR] = SMinMax{1, 12};
    c_minMaxMap[DAY_OF_WEEK] = SMinMax{0, 6};

    std::vector<std::string> l = split(c_cronspec, ' ');

    if(l.size() != 5)
        return false;

    for(const std::pair<ECronPart, SMinMax> &part : c_minMaxMap)
    {
        if(!initPart(part, l.at(part.first)))
            return false;
    }
    c_isValid = true;
    return true;
}

bool CCronCalculator::initPart(const std::pair<ECronPart, SMinMax> &part, const std::string &crontabPartSpec)
{
    std::vector<int> list;
    if(crontabPartSpec == "*")
    {
        for(int i = part.second.min; i <= part.second.max; i++)
            list.push_back(i);

        c_partsMap[part.first] = list;
    }
    else if(crontabPartSpec.find('/') != std::string::npos)
    {
        std::vector<std::string> l = split(crontabPartSpec, '/');

        if(l.size() != 2)
            return false;

        if(l.front() != "*")
            return false;

        bool ok = false;
        int val = toInt(l.back(), &ok);

        if(!ok || (val < part.second.min) || (val > part.second.max) || (val < 2))
            return false;

        for(int i = val; i <= part.second.max; i+=val)
            list.push_back(i);

        c_partsMap[part.first] = list;
    }
    else if(crontabPartSpec.find('-') != std::string::npos)
    {
        std::vector<std::string> l = split(crontabPartSpec, '-');

        if(l.size() != 2)
            return false;

        bool ok = false;
        int min = toInt(l.front(), &ok);

        if(!ok)
            return false;

        int max = toInt(l.back(), &ok);

        if(!ok)
            return false;

        if((min >= max) || (min < part.second.min) || (max > part.second.max))
            return false;

        for(int i = min; i <= max; i++)
            list.push_back(i);

        c_partsMap[part.first] = list;
    }
    else if(crontabPartSpec.find(',') != std::string::npos)
    {
        std::vector<std::string> l = split(crontabPartSpec, ',');

        if(l.size() < 2)
            return false;

        bool ok = false;
        int val = 0;
        for(const std::string &s : l)
        {
            val = toInt(s, &ok);
            if(!ok || (val < part.second.min) || (val > part.second.max))
                return false;
            list.push_back(val);
        }

        std::sort(list.begin(), list.end());

        c_partsMap[part.first] = list;
    }
    else
    {
        bool ok = false;
        int val = toInt(crontabPartSpec, &ok);

        if(!ok)
            return false;

        list.push_back(val);
        c_partsMap[part.first] = list;
    }
    return true;
}

int CCronCalculator::getNextTo(const ECronPart part, const int val, bool *pOverflow)
{
    if(pOverflow)
        *pOverflow = false;

    for(const auto &item : c_partsMap.at(part))
    {
        if(item >= val)
            return item;
    }

    if(pOverflow)
        *pOverflow = true;

    return c_partsMap.at(part).at(0);
}


int CCronCalculator::toInt(const std::string &s, bool *pOk)
{
    int i = 0;

    if(pOk)
        *pOk = true;

    try
    {
        i = std::stoi(s);
    }
    catch(std::invalid_argument &e)
    {
        if(pOk)
            *pOk = false;
    }
    catch(std::out_of_range &e)
    {
        if(pOk)
            *pOk = false;
    }

    return i;
}

tm CCronCalculator::toTm(const time_t &t, bool bUTC)
{
    tm tmp;

    if(bUTC)
        SAFEGMTIME(&t, &tmp);
    else
        SAFELOCALTIME(&t, &tmp);

    return tmp;
}
