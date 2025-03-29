#include "SequentialCommandProcessor.h"
#include "../core/CommunicationManager/Model/Command.h"
#include "../core/CommunicationManager/Model/Response.h"
#include "CommandDispatcher.h"
#include <QMetaObject>
#include "LogManager.h"

SequentialCommandProcessor::SequentialCommandProcessor(QSharedPointer<CommandDispatcher> dispatcher, std::shared_ptr<spdlog::logger> logger_, QObject* parent)
    : QObject(parent)
    , m_dispatcher(dispatcher)
    , m_processing(false)
    , m_logger(logger_)
{
    // Kontrol et
    if (!m_dispatcher) {
        SpdlogWarn(m_logger, "SequentialCommandProcessor: CommandDispatcher tanımlanmamış!");
    }
}

bool SequentialCommandProcessor::isQueueEmpty() const
{
    QMutexLocker locker(&m_mutex);
    return m_queue.isEmpty();
}

void SequentialCommandProcessor::addCommand(
    std::shared_ptr<Command> command,
    std::function<void(std::shared_ptr<Response>)> callback,
    int timeoutMs)
{
    // Komutu kuyruğa ekle
    {
        QMutexLocker locker(&m_mutex);
        m_queue.enqueue({command, callback, timeoutMs});
    }

    // Eğer işlem başlatılmamışsa, başlat
    if (!m_processing) {
        processNextCommand();
    }
}

void SequentialCommandProcessor::clearQueue()
{
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
    m_processing = false;
}

void SequentialCommandProcessor::processCommands(
    const QList<std::shared_ptr<Command>>& commands,
    const QList<std::function<void(std::shared_ptr<Response>)>>& callbacks,
    int timeoutMs)
{
    // Komut ve callback listeleri aynı uzunlukta olmalı
    if (commands.size() != callbacks.size()) {
        SpdlogWarn(m_logger, "SequentialCommandProcessor: Komut ve callback listeleri aynı uzunlukta değil!");
        return;
    }

    // Tüm komutları kuyruğa ekle
    {
        QMutexLocker locker(&m_mutex);
        for (int i = 0; i < commands.size(); ++i) {
            m_queue.enqueue({commands[i], callbacks[i], timeoutMs});
        }
    }

    // Eğer işlem başlatılmamışsa, başlat
    if (!m_processing) {
        processNextCommand();
    }
}

void SequentialCommandProcessor::processNextCommand()
{
    CommandEntry entry;

    // Kuyruktaki bir sonraki komutu al
    {
        QMutexLocker locker(&m_mutex);

        if (m_queue.isEmpty()) {
            m_processing = false;
            return;
        }

        m_processing = true;
        entry = m_queue.dequeue();
    }

    // Komutu gönder ve callback'i düzenle
    if (m_dispatcher) {
        // Orijinal callback'i kaydet
        auto originalCallback = entry.callback;

        // Yeni bir callback oluştur
        auto wrappedCallback = [this, originalCallback](std::shared_ptr<Response> response) {
            // Önce orijinal callback'i çağır
            if (originalCallback) {
                originalCallback(response);
            }

            // Sonra bir sonraki komutu işle (QueuedConnection kullanarak ana thread'de çalışmasını sağla)
            QMetaObject::invokeMethod(this, &SequentialCommandProcessor::processNextCommand,
                                      Qt::QueuedConnection);
        };

        // Komutu gönder
        bool success = m_dispatcher->sendCommandAsync(entry.command, wrappedCallback, entry.timeoutMs);

        if (!success) {
            // Gönderme başarısız olduysa bir sonraki komutu dene
            SpdlogWarn(m_logger, "SequentialCommandProcessor: Komut gönderilemedi:"  + entry.command->toString().toStdString());
            QMetaObject::invokeMethod(this, &SequentialCommandProcessor::processNextCommand,
                                      Qt::QueuedConnection);
        }
    } else {
       SpdlogWarn(m_logger, "SequentialCommandProcessor: CommandDispatcher tanımlanmamış, komut gönderilemedi!");
        m_processing = false;
    }
}
