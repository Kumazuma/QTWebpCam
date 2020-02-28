#include "editpresenter.h"
#include <QtConcurrent>
EditPresenter::EditPresenter(FileImageStore* store, QObject *parent) :
    QObject(parent),
    m_model(store),
    m_timer(),
    m_elapsedTimer()
{
    store->setParent(this);
    connect(&m_timer, &QTimer::timeout, this, &EditPresenter::timer);
}

EditPresenter::~EditPresenter()
{
    m_future.cancel();
    m_future.waitForFinished();
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

std::optional<size_t> EditPresenter::currentFrameIndex()
{
    return m_model.selectedIndex();
}

bool EditPresenter::isPlaying()
{
    return m_model.isPlay();
}

void EditPresenter::saveAnimWebp(const QString& filePath)
{
    m_model.setFilePath(filePath);
    WebpEncoder* encoder = new WebpEncoder(m_model.store()->imageSize());
    QObject::connect(encoder, &WebpEncoder::doneAFrame, this, &EditPresenter::doneAFrame);
    QObject::connect(encoder, &WebpEncoder::finish, this, &EditPresenter::finishEncode);

    auto f = [this](WebpEncoder* encoder)->WebpEncoder*
    {
        auto store = this->m_model.store();
        for(size_t i = 0; i < store->size(); i++)
        {
            auto frame = *store->at(i);
            QImage img = store->getImage(frame);
            encoder->putFrame(img, frame.duration());
        }
        encoder->end();
        return encoder;
    };
    m_future = QtConcurrent::run(f, encoder);

}

void EditPresenter::play()
{
    m_model.setPlay(true);
    emit changePlayState(m_model.isPlay());
    auto it = m_model.selectedIndex();
    if(!it)
    {
        it = 0;
        m_model.setSelectedIndex(it);
        emit currentImageFrame(*m_model.store()->at(0));
    }
    m_leftDuration = m_model.store()->at(*it)->duration();
    m_timer.start(2);
    m_elapsedTimer.restart();
    m_lastTick = m_elapsedTimer.elapsed();
}

void EditPresenter::stop()
{
    m_model.setPlay(false);
    m_timer.stop();

    emit changePlayState(m_model.isPlay());
}

void EditPresenter::timer()
{
    auto now = m_elapsedTimer.elapsed();
    auto delta = now - m_lastTick;
    auto index = *m_model.selectedIndex();
    m_lastTick = now;
    while(delta > m_leftDuration)
    {
        delta -= m_leftDuration;
        index = index + 1;
        auto frame = m_model.store()->at(index);
        if(!frame)
            index = 0;
        m_leftDuration = m_model.store()->at(index)->duration();
        m_model.setSelectedIndex(index);
    }
    emit currentImageFrame(*m_model.store()->at(index));
    m_leftDuration -= delta;
}

void EditPresenter::doneAFrame(int current)
{

}

void EditPresenter::finishEncode()
{
    m_future.waitForFinished();
    auto encoder = m_future.result();
    QFile file(m_model.filePath());
    file.open(QFile::ReadWrite);
    encoder->result(file);
    emit completeFileSave(m_model.filePath());
    delete encoder;
}

void EditPresenter::threadRun()
{

}
