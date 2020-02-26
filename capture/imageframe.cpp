#include "imageframe.h"
#include "fileimagestore.h"
ImageFrame::ImageFrame(
        FileImageStore& imageStore,
        size_t offset,
        size_t size,
        size_t formatIndex,
        QSize imgSize,
        size_t duration):
    m_imageStore(imageStore),
    m_offset(offset),
    m_size(size),
    m_formatIndex(formatIndex),
    m_imgSize(imgSize),
    m_duration(duration)
{

}
