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
    void stopEncode();
protected:
    void run() override;

protected slots:

signals:
    void progress(int value, int total);
};
#endif // ENCODINGTHREAD_H
