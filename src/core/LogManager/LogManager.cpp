#include "LogManager.h"
#include <iostream>
#include "Utils.h"
#include "Constants.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int LogManager::loggerCount = 0;
std::filesystem::path LogManager::logPath = Constants::Log::LOG_DIR;

LogManager::LogManager(const std::string &_loggerName)
{
    this->loggerName = _loggerName;
    this->logLevel = spdlog::level::trace;
    this->logger = nullptr;
    this->logPattern = "[%D %T.%f][%n][%l][%P/%t][%s:%#] %v";

    if (LogManager::loggerCount == 0) {
        spdlog::flush_every(std::chrono::seconds(3));

        auto date = Utils::getCurrentDate();
        auto time = Utils::getCurrentTime();
        LogManager::logPath = LogManager::logPath / date / time;
    }
    LogManager::loggerCount++;
}

LogManager::~LogManager()
{
    spdlog::shutdown();
}

std::string LogManager::getLoggerName()
{
    return this->loggerName;
}

spdlog::level::level_enum LogManager::getLogLevel()
{
    return this->logLevel;
}

std::shared_ptr<spdlog::logger> LogManager::getLogger()
{
    return std::move(this->logger);
}

std::string LogManager::getLogPattern()
{
    return this->logPattern;
}

void LogManager::setLoggerName(const std::string &_loggerName)
{
    this->loggerName = _loggerName;
}

void LogManager::setLogLevel(spdlog::level::level_enum _logLevel)
{
    this->logLevel = _logLevel;
}

void LogManager::setLogger(std::shared_ptr<spdlog::logger> _logger)
{
    this->logger = std::move(_logger);
}

void LogManager::setLogPattern(const std::string &_logPattern)
{
    this->logPattern = _logPattern;
}

void LogManager::addFileSink(const std::string &logFileName)
{
    auto max_size = Constants::Log::ROTATING_LIMIT;
    auto max_files = Constants::Log::ROTATING_FILE_COUNT;

    std::filesystem::path currentLogPath = LogManager::logPath / logFileName;
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(currentLogPath.string(),
                                                                           max_size, max_files);

    fileSink->set_level(spdlog::level::trace);
    this->sinks.push_back(fileSink);
}

void LogManager::addConsoleSink()
{
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::trace);
    this->sinks.push_back(consoleSink);
}

std::shared_ptr<spdlog::logger> LogManager::start()
{
    try {
        this->logger = std::make_shared<spdlog::logger>(this->loggerName, this->sinks.begin(),
                                                        this->sinks.end());
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return nullptr;
    }
    this->logger->set_level(spdlog::level::trace);
    this->logger->set_pattern(this->logPattern);
    spdlog::register_logger(this->logger);

    return this->logger;
}
