#include "fileimagestore.h"
#include "imageframe.h"
FileImageStoreBuilder::FileImageStoreBuilder(QSize imgSize, QObject *parent) :
    QObject(parent),
    m_tempFile(new QTemporaryFile(parent)),
    m_imgSize(imgSize)
{
    if(!m_tempFile->open())
    {
        //TODO: 예외처리 해야 함
        qDebug()<<m_tempFile->errorString();
    }
}

FileImageStore *FileImageStoreBuilder::buildStore(QObject *parent)
{
    return new FileImageStore(m_tempFile,
                              m_imgSize,
                              std::move(m_offsets),
                              std::move(m_pixelformats),
                              parent);
}

void FileImageStoreBuilder::pushBack(QImage image, int duration)
{

    auto * data =  image.bits();
    auto len = image.sizeInBytes();
    char* compressedData = new char[len];
    ulong compressedDataLen = len;
    compress2((Byte*)compressedData, &compressedDataLen, data, len,2);

    auto pos = m_tempFile->pos();

    size_t totalWriteDataCount = 0;
    do
    {
        auto writeDataCount =  m_tempFile->write
                (compressedData + totalWriteDataCount,
                 compressedDataLen - totalWriteDataCount);
        totalWriteDataCount += writeDataCount;
    }
    while(totalWriteDataCount != compressedDataLen);
    delete[] compressedData;
    m_tempFile->flush();
    auto formatIndex = m_pixelformats.size();
    auto pixmodel = image.format();

    for(size_t i = 0 ; i <m_pixelformats.size();i++)
    {
        auto& it = m_pixelformats[i];
        if(pixmodel == it)
        {
            formatIndex = i;
            break;
        }
    }
    if(formatIndex == m_pixelformats.size())
    {
        m_pixelformats.push_back(pixmodel);
    }
    std::array<uint64_t, 4> offset;
    offset[0] = pos;
    offset[1] = compressedDataLen;
    offset[2] = formatIndex;
    offset[3] = duration;
    m_offsets.push_back(offset);
}
FileImageStore::FileImageStore(QTemporaryFile* file,
                               QSize imgSize,
                               std::vector<std::array<uint64_t,4>> && offset,
                               std::vector<QImage::Format> && pixelformats,
                               QObject *parent):
    QObject(parent),
    m_tempFile(file),
    m_imgSize(imgSize),
    m_pixelformats(pixelformats)

{   m_tempFile->flush();
    m_tempFile->setParent(this);
    auto last = *(offset.end() - 1);
    m_mappedFile = m_tempFile->map(0, last[0] + last[1]);
    for(auto& it: offset)
    {
        ImageFrame frame
                (*this,
                 it[0],
                it[1],
                it[2],
                m_imgSize,
                it[3]
                  );
        m_frames.push_back(std::move(frame));
    }
}

FileImageStore::~FileImageStore()
{
#ifndef QT_NO_DEBUG
    qDebug()<<"method: "<< __FUNCTION__;
#endif
    m_tempFile->unmap( m_mappedFile);
    m_tempFile->close();
    delete m_tempFile;
}

std::optional<ImageFrame> FileImageStore::at(size_t i) const
{
    if(i < m_frames.size())
    {
        return std::optional(m_frames[i]);
    }
    return std::nullopt;
}

uint8_t *FileImageStore::getRaw(size_t i)
{
    if(i < m_frames.size())
    {
        auto& frame = m_frames[i];
        return m_mappedFile + frame.m_offset;
    }
    return nullptr;
}

QImage FileImageStore::getImage(const ImageFrame &frame) const
{
    if(&frame.m_imageStore == this)
    {
        auto ptr = m_mappedFile;
        if(!ptr)
        {
            ptr = m_tempFile->map(0, m_tempFile->size());
            if(ptr)
                qDebug()<<m_tempFile->errorString();
            (const_cast<uint8_t*&>(m_mappedFile)) = ptr;
        }
        ulong datasize = m_imgSize.width() * m_imgSize.height() * 4;
        uchar* data = (uchar*)malloc(datasize);
        uncompress((Byte*)data, &datasize, ptr + frame.m_offset, frame.m_size);


        QImage img(data,
                   m_imgSize.width(),
                   m_imgSize.height(),
                   m_pixelformats[frame.m_formatIndex],
                FileImageStore::FreeImage,
                data);

        if(img.isNull())
        {
            return QImage();
        }
        if(!m_mappedFile && ptr)
        {
            m_tempFile->unmap(ptr);
        }
        return img;
    }
    return QImage();
}

size_t FileImageStore::size() const
{
    return m_frames.size();
}

QSize FileImageStore::imageSize()const
{
    return m_imgSize;
}

std::optional<size_t> FileImageStore::findIndex(const ImageFrame & frame)
{
    if(&frame.m_imageStore != this)
        return std::nullopt;
    for(size_t i = 0; i < m_frames.size(); i++)
    {
        auto& it = m_frames[i];
        if(it.m_offset == frame.m_offset)
        {
            return std::optional(i);
        }
    }
    return std::nullopt;
}

void FileImageStore::unload()
{
    if(m_mappedFile)
    {
        m_tempFile->unmap(m_mappedFile);
        m_mappedFile = nullptr;
    }

}

void FileImageStore::load()
{
    if(!m_mappedFile)
    {
        m_mappedFile = m_tempFile->map(0, m_tempFile->size());
    }
}

void FileImageStore::FreeImage(void *ptr)
{
    free(ptr);
}

MemoryMapStoreBuilder::MemoryMapStoreBuilder(QSize imgSize, QObject *parent):
    QObject(parent),
    m_tempFile(new QTemporaryFile(parent)),
    m_imgSize(imgSize)
{
    auto firstCapacity = m_imgSize.width() * m_imgSize.height() * 4 * 2;
    m_capacity = firstCapacity;
    m_tempFile->open();
    m_tempFile->setAutoRemove(true);

    for(int i = 0; i < firstCapacity/4; i++)
    {
        int temp = 0;
        m_tempFile->write((char*)&temp, 4);
    }
    m_tempFile->flush();
    m_mappedFile = m_tempFile->map(0, firstCapacity);
    if(!m_mappedFile)
    {
        qDebug()<<"file mapping failed";
    }
}

MemoryMapStoreBuilder::~MemoryMapStoreBuilder()
{
#ifndef QT_NO_DEBUG
    qDebug()<<"method: "<< __FUNCTION__;
#endif
    if(m_tempFile != nullptr)
    {
        m_tempFile->unmap(m_mappedFile);
        m_tempFile->close();
        delete m_tempFile;
    }
}

FileImageStore *MemoryMapStoreBuilder::buildStore(QObject *parent)
{
    auto temp = m_tempFile;
    m_tempFile->unmap(m_mappedFile);
    m_tempFile = nullptr;

    return new FileImageStore(temp,
                              m_imgSize,
                              std::move(m_offsets),
                              std::move(m_pixelformats),
                              parent);
}

void MemoryMapStoreBuilder::pushBack(QImage image, int duration)
{
    auto * data =  image.bits();
    auto len = image.sizeInBytes();
    char* compressedData = new char[len];
    ulong compressedDataLen = len;
    compress2((Byte*)compressedData, &compressedDataLen, data, len,1);

    pushBack((uint8_t*)compressedData, (size_t)compressedDataLen, image.format(), duration);
    delete[] compressedData;
}

void MemoryMapStoreBuilder::pushBack(ImageFrame &frame)
{
    auto& store = frame.m_imageStore;
    if(&store == nullptr)
    {
        return;
    }
    auto* ptr = store.m_mappedFile + frame.m_offset;
    auto lenth = frame.m_size;
    auto format = store.m_pixelformats[frame.m_formatIndex];
    auto duration = frame.m_duration;
    pushBack(ptr, lenth, format, duration);
}

void MemoryMapStoreBuilder::pushBack(uint8_t *ptr, size_t lenth, QImage::Format format, int duration)
{
    auto pos = 0;
    if(!m_offsets.empty())
    {
        auto& frame = *(m_offsets.end() - 1);
        pos = frame[0] + frame[1];
    }
    if(pos + lenth > m_capacity)
    {
        for(int i = 0; i < m_capacity/4; i++)
        {
            int temp = 0;
            m_tempFile->write((char*)&temp, 4);
        }
        m_capacity += m_capacity/2;
        m_tempFile->unmap(m_mappedFile);
        m_mappedFile = m_tempFile->map(0, m_capacity);
    }
    memcpy(m_mappedFile + pos, ptr, lenth);

    auto formatIndex = m_pixelformats.size();
    auto pixmodel = format;

    for(size_t i = 0 ; i <m_pixelformats.size();i++)
    {
        auto& it = m_pixelformats[i];
        if(pixmodel == it)
        {
            formatIndex = i;
            break;
        }
    }
    if(formatIndex == m_pixelformats.size())
    {
        m_pixelformats.push_back(pixmodel);
    }
    std::array<uint64_t, 4> offset;
    offset[0] = pos;
    offset[1] = lenth;
    offset[2] = formatIndex;
    offset[3] = duration;
    m_offsets.push_back(offset);
    emit updateStore(m_offsets.size(), pos + lenth);
}
