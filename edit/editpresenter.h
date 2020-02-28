#ifndef EDITPRESENTER_H
#define EDITPRESENTER_H

#include <QObject>
#include "editmodel.h"
#include <QTimer>
#include <QElapsedTimer>
class EditPresenter : public QObject
{
    Q_OBJECT
private:
    EditModel m_model;
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    int m_leftDuration = 0;
    qint64 m_lastTick = 0;
public:
    explicit EditPresenter(FileImageStore* store, QObject *parent = nullptr);
    FileImageStore& imageStore(){return *m_model.store();}
    void setCurrentImageFrame(const ImageFrame& frame);
    QImage getImageFromImageFrame(const ImageFrame& frame);
    std::optional<size_t> currentFrameIndex();
    bool isPlaying();
public slots:
    void play();
    void stop();
protected slots:
    void timer();
signals:
    void currentImageFrame(ImageFrame frame);
    void changePlayState(bool state);
};

#endif // EDITPRESENTER_H
