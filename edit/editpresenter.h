#ifndef EDITPRESENTER_H
#define EDITPRESENTER_H

#include <QObject>
#include "editmodel.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>
#include "webpencoder.h"
class EditPresenter : public QObject
{
    Q_OBJECT
private:
    EditModel m_model;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_leftDuration = 0;
    int m_encodingNext = 0;
    qint64 m_lastTick = 0;
    QFuture<WebpEncoder*> m_future;
    QThread m_thread;
public:
    explicit EditPresenter(FileImageStore* store, QObject *parent = nullptr);
    ~EditPresenter();
    FileImageStore& imageStore(){return *m_model.store();}
    void setCurrentImageFrame(const ImageFrame& frame);
    QImage getImageFromImageFrame(const ImageFrame& frame);
    std::optional<size_t> currentFrameIndex();
    bool isPlaying();
    void saveAnimWebp(const QString& filePath);
public slots:
    void play();
    void stop();
protected slots:
    void timer();
    void doneAFrame(int current);
    void finishEncode();
    void threadRun();
signals:
    void currentImageFrame(ImageFrame frame);
    void changePlayState(bool state);
    void completeFileSave(QString path);
};

#endif // EDITPRESENTER_H
