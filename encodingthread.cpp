#include "encodingthread.h"
#include "QAbstractEventDispatcher"
EncodingThread::EncodingThread(WebpEncoder &encoder, FileImageStore &storage, QObject *parent):
    QThread(parent), m_encoder(encoder), m_storage(storage)
{

}

void EncodingThread::run()
{
    for(int i = 0;i < m_storage.size(); i++)
    {

        this->eventDispatcher()->processEvents(QEventLoop::AllEvents);
        if(QThread::currentThread()->isInterruptionRequested() )
        {
            return;
        }
        auto frame = *m_storage.at(i);
        QImage img = m_storage.getImage(frame);
        m_encoder.putFrame(img, frame.duration());
        emit progress(i, m_storage.size());
    }
    m_encoder.end();
    return;
}

void EncodingThread::stopEncode()
{
    this->eventDispatcher()->interrupt();
}
