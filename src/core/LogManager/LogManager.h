#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <memory>
#include <string>
#include <vector>
#include <QFile>

#include "spdlog/spdlog.h"

#define SpdlogTrace(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, msg)
#define SpdlogDebug(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, msg)
#define SpdlogInfo(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, msg)
#define SpdlogWarn(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, msg)
#define SpdlogError(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, msg)
#define SpdlogCritical(logger, msg) SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, msg)

class LogManager
{
private:
    /** Name of the logger */
    std::string loggerName;
    /** Log level of the logger */
    spdlog::level::level_enum logLevel;
    /** Logger object */
    std::shared_ptr<spdlog::logger> logger;
    /** Sinks of the logger */
    std::vector<spdlog::sink_ptr> sinks;
    /** Log pattern of the logger */
    std::string logPattern;
    /** Count of the logger */
    static int loggerCount;
    /** Path of the log file */
    static  std::filesystem::path logPath;
    /** Thread pool of the logger */
    std::shared_ptr<spdlog::details::thread_pool> tp;
public:
    LogManager(const std::string &_loggerName);
    ~LogManager();

    /**
     * @brief Get the Logger Name object
     *
     * @return std::string
     */
    std::string getLoggerName();

    /**
     * @brief Get the Log Level object
     *
     * @return spdlog::level::level_enum
     */
    spdlog::level::level_enum getLogLevel();

    /**
     * @brief Get the Logger object
     *
     * @return std::shared_ptr<spdlog::logger>
     */
    std::shared_ptr<spdlog::logger> getLogger();

    /**
     * @brief Get the log pattern
     *
     * @return std::string
     */
    std::string getLogPattern();

    /**
     * @brief Set the Logger Name object
     *
     * @param loggerName Input logger name
     */
    void setLoggerName(const std::string &_loggerName);

    /**
     * @brief Set the Log Level object
     *
     * @param logLevel Input log level
     */
    void setLogLevel(spdlog::level::level_enum _logLevel);

    /**
     * @brief Set the Logger object
     *
     * @param logger Input logger object
     */
    void setLogger(std::shared_ptr<spdlog::logger> _logger);

    /**
     * @brief Set the Log Pattern object
     *
     * @param logPattern Input log pattern
     */
    void setLogPattern(const std::string &logPattern);

    /**
     * @brief Add a new file sink to the logger
     *
     * @param logFileName Input log file name
     */
    void addFileSink(const std::string &logFileName);

    /**
     * @brief Add a new console sink to the logger
     *
     */
    void addConsoleSink();

    /**
     * @brief Start the logger
     *
     * @return std::shared_ptr<spdlog::logger>
     */
    std::shared_ptr<spdlog::logger> start();
};

#endif // LOGMANAGER_H
