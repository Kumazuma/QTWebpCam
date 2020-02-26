#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H
#include <cstdint>
#include <QImage>
class FileImageStore;
class ImageFrame
{
    friend class FileImageStore;
public:

    size_t duration(){return m_duration;}
    QSize size(){return m_imgSize;}
    size_t width(){return m_imgSize.width();}
    size_t height(){return m_imgSize.height();}
    size_t sizeInBytes(){return m_size;}
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

#endif // IMAGEFRAME_H
