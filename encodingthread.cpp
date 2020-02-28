#include "encodingthread.h"

EncodingThread::EncodingThread(WebpEncoder &encoder, FileImageStore &storage, QObject *parent):
    QThread(parent), m_encoder(encoder), m_storage(storage)
{

}

void EncodingThread::run()
{
    for(int i = 0;i < m_storage.size(); i++)
    {
        if(QThread::currentThread()->isInterruptionRequested() )
        {
            qDebug()<<"isInterruptionRequested result is true";
            break;
        }
        auto frame = *m_storage.at(i);
        QImage img = m_storage.getImage(frame);
        m_encoder.putFrame(img, frame.duration());
    }
    m_encoder.end();
    return;
}
