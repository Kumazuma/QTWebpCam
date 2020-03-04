#ifndef EDITPRESENTER_H
#define EDITPRESENTER_H

#include <QObject>
#include "editmodel.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>
#include "webpencoder.h"
#include <QUndoCommand>

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
    WebpEncoder * m_encoder;
    QThread* m_thread = nullptr;
public:
    explicit EditPresenter(FileImageStore* store, QObject *parent = nullptr);
    ~EditPresenter();
    const FileImageStore& imageStore() const {return *m_model.store();}
    void setCurrentImageFrame(const ImageFrame& frame);
    QImage getImageFromImageFrame(const ImageFrame& frame) const ;
    std::optional<size_t> currentFrameIndex();
    bool isPlaying();

    void cancelEncode();
    void deleteFrame(size_t start){deleteFrame(start,start);}
    void deleteFrame(size_t start, size_t end);
    void Undo(QUndoCommand* command);
    void Redo(QUndoCommand* command);
    QUndoStack* undoStack();
public slots:
    void play();
    void stop();
    void setCropRect(QRect);
    void crop();
    void setFilePath(const QString& filePath);
    void saveAnimWebp();
protected slots:
    void timer();
    void progress(int current, int size);
    void finishEncode();
signals:
    void progressEncode(int current, int size);
    void currentImageFrame(const ImageFrame &);
    void changePlayState(bool state);
    void completeFileSave(QString path);
    void updateImageStore();
};

#endif // EDITPRESENTER_H
