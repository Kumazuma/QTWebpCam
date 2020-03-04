#include "foreachthread.h"



FrameDeleteThread::FrameDeleteThread(FileImageStore &source, MemoryMapStoreBuilder &dest, QObject *parent):
    QThread(parent),
    m_source(source),
    m_dest(dest)
{

}

void FrameDeleteThread::setStart(size_t start)
{
    m_start = start;
}

void FrameDeleteThread::setEnd(size_t end)
{
    m_end = end;
}
void FrameDeleteThread::run()
{
    for(size_t i = 0; i < m_source.size(); i++)
    {
        if(this->isInterruptionRequested())
            return;
        if(m_start <= i && i < m_end)
            continue;
        auto frame = m_source.at(i);
        m_dest.pushBack(*frame);
    }

}
ImageCropThread::ImageCropThread(FileImageStore &source, MemoryMapStoreBuilder &dest, QObject *parent):
    QThread(parent),
    m_source(source),
    m_dest(dest)
{

}

void ImageCropThread::setCropRect(const QRect &rect)
{
    m_rect = rect;
}
#include <QAbstractEventDispatcher>
void ImageCropThread::run()
{
    for(size_t i = 0; i < m_source.size(); i++)
    {
        if(this->isInterruptionRequested())
            return;
        this->eventDispatcher()->processEvents(QEventLoop::AllEvents);
        auto frame = m_source.at(i);
        auto img = m_source.getImage(*frame);
        m_dest.pushBack(img.copy(m_rect), frame->duration());
    }
}
