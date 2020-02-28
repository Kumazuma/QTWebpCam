#include "editpresenter.h"

EditPresenter::EditPresenter(FileImageStore* store, QObject *parent) :
    QObject(parent),
    m_model(store)
{
    store->setParent(this);
}

void EditPresenter::setCurrentImageFrame(const ImageFrame& frame)
{
    auto* store = m_model.store();
    auto index = store->findIndex(frame);
    if(!index)
        return;
    m_model.setSelectedIndex(index);
    emit currentImageFrame(frame);
}

QImage EditPresenter::getImageFromImageFrame(const ImageFrame &frame)
{
    auto store = m_model.store();
    return store->getImage(frame);
}

void EditPresenter::play()
{
    m_model.setPlay(true);
    emit changePlayState(m_model.isPlay());
}

void EditPresenter::stop()
{
    m_model.setPlay(false);
    emit changePlayState(m_model.isPlay());
}

void EditPresenter::timer()
{

}
