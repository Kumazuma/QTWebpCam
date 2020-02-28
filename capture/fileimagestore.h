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
class FileImageStore: public QObject
{
    Q_OBJECT
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
    FileImageStore* buildStore(QObject* parent = nullptr);
    std::optional<ImageFrame> at(size_t i) const;
    QImage getImage(const ImageFrame& frame) const;
    size_t size() const;
    QSize imageSize();
    std::optional<size_t> findIndex(const ImageFrame&);
protected:
    static void FreeImage(void* ptr);
};

class FileImageStoreBuilder : public QObject
{
    Q_OBJECT
private:
    QTemporaryFile* m_tempFile;
    QSize m_imgSize;
    std::vector<std::array<uint64_t,4>> m_offsets;
    std::vector<QImage::Format> m_pixelformats;
public:
    explicit FileImageStoreBuilder(QSize imgSize, QObject *parent = nullptr);
    FileImageStore* buildStore(QObject* parent = nullptr);
public slots:
    void pushBack(QImage pixmap, uint64_t duration);
private:

signals:

};

#endif // FILEIMAGESTORE_H
