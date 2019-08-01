#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_ERROR 2

class Logger
{
public:
    static void log(QString message, int logLevel = LOG_LEVEL_INFO);

protected:
    Logger();
};

#endif // LOGGER_H
