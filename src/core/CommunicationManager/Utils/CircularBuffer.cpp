#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(size_t capacity)
    : m_capacity(capacity)
    , m_size(0)
    , m_head(0)
    , m_tail(0)
    , m_overflowCount(0)
{
    m_capacity = std::max<size_t>(1, capacity);

    m_buffer.resize(static_cast<int>(m_capacity));
}

CircularBuffer::~CircularBuffer()
{
    QMutexLocker locker(&m_mutex);
    m_buffer.clear();
}

size_t CircularBuffer::write(const QByteArray &data)
{
    if (data.isEmpty())
        return 0;

    QMutexLocker locker(&m_mutex);

    size_t dataSize = static_cast<size_t>(data.size());

    if (dataSize > m_capacity) {
        QByteArray truncatedData = data.right(static_cast<int>(m_capacity));
        dataSize = m_capacity;
        m_overflowCount++;

        m_buffer = truncatedData;
        m_head = 0;
        m_tail = 0;
        m_size = m_capacity;

        m_dataAvailable.wakeAll();

        return dataSize;
    }

    // Buffer'da yeterli yer yoksa, eski veriyi üzerine yazacağız
    if (dataSize > (m_capacity - m_size)) {
        // Ne kadar veri üzerine yazılacağını hesapla
        size_t overwriteSize = dataSize - (m_capacity - m_size);

        // Okuma pozisyonunu ilerlet
        m_tail = (m_tail + overwriteSize) % m_capacity;

        // Büyüklüğü güncelle (kapasiteyi aşamaz)
        m_size = std::min(m_capacity, m_size + dataSize - overwriteSize);

        m_overflowCount++;
    } else {
        // Buffer'a veri sığacak
        m_size += dataSize;
    }

    // Veriyi buffer'a kopyala
    for (size_t i = 0; i < dataSize; ++i) {
        m_buffer[static_cast<int>(m_head)] = data[static_cast<int>(i)];
        m_head = nextPosition(m_head);
    }

    // Yeni veri olduğunu bildir
    m_dataAvailable.wakeAll();

    return dataSize;
}

QByteArray CircularBuffer::read(qint64 maxSize)
{
    QMutexLocker locker(&m_mutex);

    if (m_size == 0) {
        return QByteArray();
    }

    // Okunacak veri miktarını hesapla
    size_t readSize;
    if (maxSize < 0 || static_cast<size_t>(maxSize) > m_size) {
        readSize = m_size;
    } else {
        readSize = static_cast<size_t>(maxSize);
    }

    // Veriyi oku
    QByteArray result;
    result.reserve(static_cast<int>(readSize));

    for (size_t i = 0; i < readSize; ++i) {
        result.append(m_buffer[static_cast<int>(m_tail)]);
        m_tail = nextPosition(m_tail);
    }

    // Büyüklüğü güncelle
    m_size -= readSize;

    // Buffer'da yer açıldığını bildir
    m_spaceAvailable.wakeAll();

    return result;
}

QByteArray CircularBuffer::readAll()
{
    return read(-1);
}

QByteArray CircularBuffer::waitForData(int timeoutMs)
{
    QMutexLocker locker(&m_mutex);

    if (m_size == 0) {
        // Veri gelmesini bekle
        bool dataArrived = m_dataAvailable.wait(&m_mutex, timeoutMs);

        if (!dataArrived || m_size == 0) {
            return QByteArray();
        }
    }

    return read(-1);
}

void CircularBuffer::clear()
{
    QMutexLocker locker(&m_mutex);

    m_head = 0;
    m_tail = 0;
    m_size = 0;

    // Buffer'da yer açıldığını bildir
    m_spaceAvailable.wakeAll();
}

size_t CircularBuffer::size() const
{
    QMutexLocker locker(&m_mutex);
    return m_size;
}

size_t CircularBuffer::capacity() const
{
    return m_capacity;
}

size_t CircularBuffer::freeSpace() const
{
    QMutexLocker locker(&m_mutex);
    return m_capacity - m_size;
}

int CircularBuffer::fillPercentage() const
{
    QMutexLocker locker(&m_mutex);
    return static_cast<int>((m_size * 100) / m_capacity);
}

BufferState CircularBuffer::state() const
{
    QMutexLocker locker(&m_mutex);
    return calculateState();
}

bool CircularBuffer::hasOverflowed() const
{
    QMutexLocker locker(&m_mutex);
    return m_overflowCount > 0;
}

void CircularBuffer::resetOverflowCounter()
{
    QMutexLocker locker(&m_mutex);
    m_overflowCount = 0;
}

int CircularBuffer::overflowCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_overflowCount;
}

size_t CircularBuffer::nextPosition(size_t position) const
{
    return (position + 1) % m_capacity;
}

BufferState CircularBuffer::calculateState() const
{
    if (m_size == 0) {
        return BufferState::Empty;
    } else if (m_size == m_capacity) {
        return BufferState::Full;
    } else if (m_overflowCount > 0) {
        return BufferState::Overflow;
    } else {
        return BufferState::Normal;
    }
}
