#pragma once

#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <memory>

/**
     * @brief Tampon durumunu izlemek için kullanılan enum
     */
enum class BufferState {
    Empty,      ///< Tampon boş
    Normal,     ///< Tampon kısmen dolu
    Full,       ///< Tampon dolu
    Overflow    ///< Taşma oldu
};

/**
 * @brief Yüksek performanslı döngüsel (dairesel) tampon sınıfı
 *
 * Bu sınıf, thread-safe bir şekilde veri depolamak ve almak için kullanılan
 * döngüsel bir tampon (ring buffer) implementasyonudur. Yüksek frekanslı seri haberleşme
 * için optimize edilmiştir.
 *
 * @details
 * CircularBuffer, veri kaybını önlemek ve yüksek veri hızlarında bile
 * stabil bir performans sağlamak için tasarlanmıştır. C++20 özellikleri ve
 * modern multi-threading teknikleri kullanılarak geliştirilmiştir.
 */
class CircularBuffer {
public:
    /**
     * @brief Belirtilen kapasitede bir CircularBuffer oluşturur
     * @param capacity Tampon kapasitesi (bayt cinsinden)
     */
    explicit CircularBuffer(size_t capacity);

    /**
     * @brief CircularBuffer nesnesini yok eder
     */
    ~CircularBuffer();

    /**
     * @brief Tampona veri yazar
     *
     * Bu metod thread-safe olup, birden fazla thread tarafından güvenle çağrılabilir.
     * Tampon doluysa, en eski veriler üzerine yazılır (dairesel tampon mantığı).
     *
     * @param data Yazılacak veri
     * @return Yazılan bayt sayısı
     */
    size_t write(const QByteArray& data);

    /**
     * @brief Tampondan belirtilen miktarda veri okur
     *
     * Bu metod thread-safe olup, birden fazla thread tarafından güvenle çağrılabilir.
     * Eğer yeterli veri yoksa, mevcut verileri döndürür.
     *
     * @param maxSize Okunacak maksimum bayt sayısı, -1 ise tüm mevcut verileri okur
     * @return Okunan veriler
     */
    QByteArray read(qint64 maxSize = -1);

    /**
     * @brief Tampondaki tüm verileri okur
     *
     * Bu metod thread-safe olup, birden fazla thread tarafından güvenle çağrılabilir.
     * Tampondaki tüm verileri okur ve tamponu temizler.
     *
     * @return Tamponda bulunan tüm veriler
     */
    QByteArray readAll();

    /**
     * @brief Belirtilen süre boyunca veri gelmesini bekler
     *
     * Bu metod, tampon boşsa ve belirtilen süre içinde yeni veri gelmezse,
     * boş bir QByteArray döndürür.
     *
     * @param timeoutMs Bekleme süresi (milisaniye)
     * @return Okunan veriler veya zaman aşımında boş QByteArray
     */
    QByteArray waitForData(int timeoutMs);

    /**
     * @brief Tamponu temizler
     */
    void clear();

    /**
     * @brief Tamponda bulunan veri miktarını döndürür
     * @return Tampondaki veri miktarı (bayt cinsinden)
     */
    size_t size() const;

    /**
     * @brief Tampon kapasitesini döndürür
     * @return Tampon kapasitesi (bayt cinsinden)
     */
    size_t capacity() const;

    /**
     * @brief Tamponda kalan boş alan miktarını döndürür
     * @return Kalan boş alan (bayt cinsinden)
     */
    size_t freeSpace() const;

    /**
     * @brief Tamponun doluluk yüzdesini döndürür
     * @return Doluluk yüzdesi (0-100)
     */
    int fillPercentage() const;

    /**
     * @brief Tampon durumunu döndürür
     * @return Tampon durumu
     */
    BufferState state() const;

    /**
     * @brief Tampon taşması olup olmadığını döndürür
     * @return Taşma olduysa true, olmadıysa false
     */
    bool hasOverflowed() const;

    /**
     * @brief Taşma sayacını sıfırlar
     */
    void resetOverflowCounter();

    /**
     * @brief Taşma sayısını döndürür
     * @return Taşma sayısı
     */
    int overflowCount() const;

private:
    QByteArray m_buffer;         ///< Veri tamponu
    size_t m_capacity;           ///< Tampon kapasitesi
    size_t m_size;               ///< Tamponda bulunan veri miktarı
    size_t m_head;               ///< Yazma pozisyonu
    size_t m_tail;               ///< Okuma pozisyonu
    int m_overflowCount;         ///< Taşma sayacı

    mutable QMutex m_mutex;      ///< Thread güvenliği için mutex
    QWaitCondition m_dataAvailable;  ///< Veri eklenmesini beklemek için koşul değişkeni
    QWaitCondition m_spaceAvailable; ///< Yer açılmasını beklemek için koşul değişkeni

    inline size_t nextPosition(size_t position) const;
    BufferState calculateState() const;
};

using CircularBufferPtr = std::shared_ptr<CircularBuffer>;
