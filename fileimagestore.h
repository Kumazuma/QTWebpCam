#ifndef FILEIMAGESTORE_H
#define FILEIMAGESTORE_H

#include <QObject>
#include <QTemporaryFile>
#include <QDataStream>
#include <zlib.h>
#include <vector>
#include <array>
#include <QPixmap>
#include <QFuture>
#include "imageframe.h"
class MemoryMapStoreBuilder;
class FileImageStore: public QObject
{
    Q_OBJECT
    friend class MemoryMapStoreBuilder;
private:
    QTemporaryFile* m_tempFile;
    QSize m_imgSize;
    std::vector<ImageFrame> m_frames;
    std::vector<QImage::Format> m_pixelformats;
    uint8_t* m_mappedFile;
    FileImageStore(const FileImageStore&) = delete;
    FileImageStore(FileImageStore&&) = delete;

public:
    explicit FileImageStore(QTemporaryFile* file,
                            QSize imgSize,
                            std::vector<std::array<uint64_t,4>> && offset,
                            std::vector<QImage::Format> && pixelformats,
                            QObject *parent = nullptr);
    virtual ~FileImageStore();
    FileImageStore* buildStore(QObject* parent = nullptr);
    std::optional<ImageFrame> at(size_t i) const;

    QImage getImage(const ImageFrame& frame) const;
    size_t size() const;
    QSize imageSize() const;
    std::optional<size_t> findIndex(const ImageFrame&);
    void unload();
    void load();
protected:
    uint8_t* getRaw(size_t i);
    static void FreeImage(void* ptr);
};
class MemoryMapStoreBuilder:public QObject
{
    Q_OBJECT
private:
    QTemporaryFile* m_tempFile;
    QSize m_imgSize;
    std::vector<std::array<uint64_t,4>> m_offsets;
    std::vector<QImage::Format> m_pixelformats;
public:
    explicit MemoryMapStoreBuilder(QSize imgSize, QObject *parent = nullptr);
    ~MemoryMapStoreBuilder();
    FileImageStore* buildStore(QObject* parent = nullptr);
    void pushBack(ImageFrame& frame);
public slots:
    void pushBack(QImage image, int duration);

protected:
    void pushBack(uint8_t* ptr, size_t lenth, QImage::Format format, int duration);
signals:
    void updateStore(int, quint64);
};

#endif // FILEIMAGESTORE_H
