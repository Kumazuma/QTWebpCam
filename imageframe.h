#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H
#include <cstdint>
#include <QImage>
class FileImageStore;
class MemoryMapStoreBuilder;
class ImageFrame
{
    friend class FileImageStore;
    friend class MemoryMapStoreBuilder;
public:
    ImageFrame():
    m_imageStore(*(FileImageStore*)nullptr){

    }
    size_t duration() const{return m_duration;}
    QSize size() const{return m_imgSize;}
    size_t width() const{return m_imgSize.width();}
    size_t height() const{return m_imgSize.height();}
    size_t sizeInBytes() const{return m_size;}
private:

    ImageFrame(
            FileImageStore& imageStore,
            size_t offset,
            size_t size,
            size_t formatIndex,
            QSize imgSize,
            size_t duration);
private:
    FileImageStore& m_imageStore;
    size_t m_offset;
    size_t m_size;
    size_t m_formatIndex;
    QSize m_imgSize;
    size_t m_duration;
};
Q_DECLARE_METATYPE(ImageFrame)
#endif // IMAGEFRAME_H
