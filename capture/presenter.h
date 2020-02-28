#ifndef CAPTURE_PRESENTER_H
#define CAPTURE_PRESENTER_H

#include <QObject>
#include "capturemodel.h"
#include <QThread>
#include <QTimer>
#include "./capturer.h"
#include "./fileimagestore.h"
class Presenter : public QObject
{
    Q_OBJECT
private:
    Capture::Model m_model;

    Capturer * m_capture = nullptr;
    FileImageStoreBuilder * m_builder = nullptr;
    QTimer * m_timer = nullptr;
    QThread m_thread;

public:
    explicit Presenter(QObject * parent = nullptr);
    ~Presenter();
    int fps() const;
    QRect recoredRect() const;
public slots:
    void setFps(int val);
    void setRecoredRect(const QRect& rect);
    void startCapture();
    void stopCapture();
private:
    void onExitThread();
signals:
    void changedRect(const QRect& rect);
    void startRecord();
    void finishRecord(FileImageStore* store);
};


#endif // CAPTURE_PRESENTER_H
