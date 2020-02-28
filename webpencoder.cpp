#include "webpencoder.h"

WebpEncoder::WebpEncoder(const QSize& imgSize, QObject *parent) :
    QObject(parent),
    m_imgSize(imgSize)
{
    WebPConfigInit(&m_config);
    WebPAnimEncoderOptionsInit(&m_encoderOption);
    m_timestamp = 0;
    //WebP Config세팅
    m_config.quality = 80.f;
    m_config.thread_level = 1;//멀티 스레드 사용
    m_config.lossless = 0;//손실포맷
    m_config.low_memory = 0;
    m_config.filter_strength = 0;//필터 잘 모르겠습
    m_config.method = 6;//압축률 최대
    //키프레임 최소와 최대;
    m_encoderOption.kmin = 1;
    m_encoderOption.kmax = 40;
    //인코더를 초기화
    m_encoder = WebPAnimEncoderNew(imgSize.width(), imgSize.height(),&m_encoderOption);
    //프레임을 초기화한다
    WebPPictureInit(&m_frame);
    m_frame.width = m_imgSize.width();
    m_frame.height = m_imgSize.height();
    m_frame.use_argb = 1;
    WebPPictureAlloc(&m_frame);
}

WebpEncoder::~WebpEncoder()
{
    WebPDataClear(&m_webpData);
    if(m_webpMux != nullptr)
    {
        WebPMuxDelete(m_webpMux);
        m_webpMux = nullptr;
    }
    if(m_encoder != nullptr)
    {
        WebPAnimEncoderDelete(m_encoder);
        m_encoder = nullptr;
    }
    WebPPictureFree(&m_frame);

}

void WebpEncoder::result(QIODevice &device)
{
    size_t totalWriteCount = 0;
    do
    {
        size_t len = m_webpData.size - totalWriteCount;
        auto ptr = (char*)m_webpData.bytes + totalWriteCount;
        int writeCount = 0;
        writeCount = device.write(ptr,len);
        if(writeCount > 0)
        {
            totalWriteCount += writeCount;
        }
    }
    while(totalWriteCount != m_webpData.size);
}

void WebpEncoder::putFrame(const QImage &image, size_t duration)
{
    auto img = image.convertToFormat(QImage::Format_RGB888);
    WebPPictureImportRGB(&m_frame, img.bits(), 3 * m_frame.width);
    WebPAnimEncoderAdd(m_encoder, &m_frame, m_timestamp, &m_config);
    m_timestamp += duration;
    emit doneAFrame(++m_count);
}

void WebpEncoder::end()
{
    WebPDataInit(&m_webpData);
    //끝나면 NULL프레임을 집어넣는다.
    WebPAnimEncoderAdd(m_encoder, nullptr, m_timestamp, nullptr);
    WebPAnimEncoderAssemble(m_encoder, &m_webpData);

    m_webpMux = WebPMuxCreate(&m_webpData, 1);
    WebPDataClear(&m_webpData);
    WebPMuxAssemble(m_webpMux, &m_webpData);
    emit finish();
}
