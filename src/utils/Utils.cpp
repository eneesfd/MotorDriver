#include "Utils.h"
#include <QColor>

Utils::Utils(QObject *parent)
    : QObject{parent}
{}

std::filesystem::path Utils::getCurrentDate()
{
    // Tarihi string olarak oluşturup path'e dönüştürme
    // Örnek: YYYY-MM-DD formatında
    std::time_t t = std::time(nullptr);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buffer[11]; // "YYYY-MM-DD" için yeterli alan
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm);
    return std::filesystem::path(buffer);
}

std::filesystem::path Utils::getCurrentTime()
{
    // Zamanı string olarak oluşturup path'e dönüştürme
    // Örnek: HH-MM-SS formatında
    std::time_t t = std::time(nullptr);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buffer[9]; // "HH-MM-SS" için yeterli alan
    std::strftime(buffer, sizeof(buffer), "%H-%M-%S", &tm);
    return std::filesystem::path(buffer);
}

QString Utils::generateUniqueId()
{
    QUuid uuid = QUuid::createUuid();
    return uuid.toString();
}
