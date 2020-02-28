#ifndef EDITPRESENTER_H
#define EDITPRESENTER_H

#include <QObject>
#include "editmodel.h"
class EditPresenter : public QObject
{
    Q_OBJECT
private:
    EditModel m_model;
public:
    explicit EditPresenter(FileImageStore* store, QObject *parent = nullptr);
    FileImageStore& imageStore(){return *m_model.store();}
    void setCurrentImageFrame(const ImageFrame& frame);
    QImage getImageFromImageFrame(const ImageFrame& frame);

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
