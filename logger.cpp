#include "logger.h"

#include <QFile>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>

#define LOG_LEVEL_INFO_STRING    "INFO   "
#define LOG_LEVEL_WARNING_STRING "WARNING"
#define LOG_LEVEL_ERROR_STRING   "ERROR  "

Logger::Logger()
{

}

void Logger::log(QString message, int logLevel)
{
    QString time = QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss");
    QString level = LOG_LEVEL_INFO_STRING;

    if (logLevel == LOG_LEVEL_WARNING)
        level = LOG_LEVEL_WARNING_STRING;
    else if (logLevel == LOG_LEVEL_ERROR)
        level = LOG_LEVEL_ERROR_STRING;

    QString fullMessage = QString("%1 %2 %3\n").arg(time).arg(level).arg(message);

    QString logFileName = qApp->applicationName() + ".log";
    QFile file(qApp->applicationDirPath() + QDir::separator() + logFileName);
    if (!file.open(QFile::WriteOnly | QFile::Append))
        return;

    file.write(fullMessage.toUtf8());
    file.close();
}
