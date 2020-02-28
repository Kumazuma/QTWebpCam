#include <QObject>
#include <QImage>

#ifndef QWEBPENCODER_H
#define QWEBPENCODER_H

#include "webp/encode.h"
#include "webp/mux.h"
class WebpEncoder : public QObject
{
    Q_OBJECT

private:
    WebPAnimEncoder* m_encoder;
    WebPAnimEncoderOptions m_encoderOption;
    WebPConfig m_config;
    WebPPicture m_frame;
    WebPData m_webpData;
    WebPMux* m_webpMux;
    size_t m_timestamp;
    QSize m_imgSize;
    size_t m_count;
public:
    explicit WebpEncoder(const QSize& imgSize, QObject *parent = nullptr);
    ~WebpEncoder();
    void result(QIODevice& device);
public slots:
    void putFrame(const QImage& image, size_t duration);
    void end();
signals:
    void finish();
    void doneAFrame(int current);
};

#endif // QWEBPENCODER_H
