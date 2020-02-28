#ifndef ENCODINGTHREAD_H
#define ENCODINGTHREAD_H
#include <QObject>
#include <QThread>
#include "webpencoder.h"
#include "capture/fileimagestore.h"
class EncodingThread: public QThread
{
    Q_OBJECT
private:
    WebpEncoder& m_encoder;
    FileImageStore & m_storage;
public:
    EncodingThread(WebpEncoder& encoder, FileImageStore & storage, QObject* parent = nullptr);
protected:
    void run() override;
};
#endif // ENCODINGTHREAD_H
