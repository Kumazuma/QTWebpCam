#include "editpresenter.h"
#include "../encodingthread.h"
#include "../foreachthread.h"
#include "editdeletecommand.h"
#include "editcropcommand.h"
EditPresenter::EditPresenter(FileImageStore* store, QObject *parent) :
    QObject(parent),
    m_model(store),
    m_timer(),
    m_elapsedTimer()
{
    store->setParent(this);
    QUndoStack * undostack = new QUndoStack(this);
    m_model.setUndoStack(undostack);
    connect(&m_timer, &QTimer::timeout, this, &EditPresenter::timer);
}

EditPresenter::~EditPresenter()
{
#ifndef QT_NO_DEBUG
    qDebug()<<"method: "<< __FUNCTION__;
#endif
    m_model.undoStack()->clear();
    delete m_model.undoStack();
    if(m_thread != nullptr)
    {
        m_thread->requestInterruption();
        m_thread->wait();
        delete m_thread;
        m_thread = nullptr;
    }
}

void EditPresenter::setCurrentImageFrame(const ImageFrame& frame)
{
    qDebug()<<__PRETTY_FUNCTION__;
    std::shared_ptr<FileImageStore> store = m_model.store();
    auto index = store->findIndex(frame);
    if(!index)
        return;
    m_model.setSelectedIndex(index);
    emit currentImageFrame(frame);
}

QImage EditPresenter::getImageFromImageFrame(const ImageFrame &frame) const
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

void EditPresenter::setFilePath(const QString& filePath)
{
    if(m_thread != nullptr)
        return;
    m_model.setFilePath(filePath);


}

void EditPresenter::saveAnimWebp()
{
    m_encoder = new WebpEncoder(m_model.store()->imageSize(), this);

    m_thread = new EncodingThread(*m_encoder, *m_model.store(),this);
    QObject::connect((EncodingThread*)m_thread, &EncodingThread::progress, this, &EditPresenter::progressEncode);
    QObject::connect(m_encoder, &WebpEncoder::finish, this, &EditPresenter::finishEncode);
    m_thread->start();
}

void EditPresenter::cancelEncode()
{
    m_thread->requestInterruption();
    m_thread->wait();
    delete m_thread;
    m_thread = nullptr;
}

void EditPresenter::deleteFrame(size_t start, size_t end)
{
    auto storeBuilder = new MemoryMapStoreBuilder(m_model.store()->imageSize(), this);
    auto thread = new FrameDeleteThread(*m_model.store(), *storeBuilder, this);
    thread->setStart(start);
    thread->setEnd(end);
    thread->start();
    thread->wait();
    thread->deleteLater();
    auto undoStack = m_model.undoStack();
    undoStack->push(new EditDeleteCommand(*this,std::shared_ptr<FileImageStore>( storeBuilder->buildStore(this)), start, end));
    delete storeBuilder;
    if(m_model.isPlay())
    {
        m_model.setPlay(false);
        m_timer.stop();
        emit changePlayState(m_model.isPlay());
    }
    m_model.setSelectedIndex(std::nullopt);
    emit updateImageStore();
}

void EditPresenter::Undo(QUndoCommand *command)
{
    auto* deleteCommand = dynamic_cast<EditDeleteCommand*>(command);
    auto* cropCommand = dynamic_cast<EditCropCommand*>(command);
    if(deleteCommand)
    {
        m_model.store() = deleteCommand->swap(m_model.store());
        emit updateImageStore();
    }
    if(cropCommand)
    {
        m_model.store() = cropCommand->swap(m_model.store());
        emit updateImageStore();
    }
}

void EditPresenter::Redo(QUndoCommand *command)
{
    auto* deleteCommand = dynamic_cast<EditDeleteCommand*>(command);
    auto* cropCommand = dynamic_cast<EditCropCommand*>(command);
    if(deleteCommand)
    {
        m_model.store() = deleteCommand->swap(m_model.store());
        emit updateImageStore();
    }
    if(cropCommand)
    {
        m_model.store() = cropCommand->swap(m_model.store());
        emit updateImageStore();
    }
}

QUndoStack *EditPresenter::undoStack()
{
    return m_model.undoStack();
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

void EditPresenter::setCropRect(QRect val)
{
    m_model.setCropRect(val);
}

void EditPresenter::crop()
{
    if(m_model.cropRect().size() == m_model.store()->imageSize())
        return;
    auto storeBuilder = new MemoryMapStoreBuilder(m_model.cropRect().size(), this);
    auto thread = new ImageCropThread(*m_model.store(), *storeBuilder, this);
    thread->setCropRect(m_model.cropRect());
    thread->start();
    thread->wait();
    thread->deleteLater();
    auto undoStack = m_model.undoStack();
    undoStack->push(new EditCropCommand(*this, std::shared_ptr<FileImageStore>(storeBuilder->buildStore(this)), m_model.cropRect()));
    delete storeBuilder;
    if(m_model.isPlay())
    {
        m_model.setPlay(false);
        m_timer.stop();
        emit changePlayState(m_model.isPlay());
    }
    m_model.setSelectedIndex(std::nullopt);
    emit updateImageStore();
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

void EditPresenter::progress(int current, int size)
{
    emit progressEncode(current, size);
}

void EditPresenter::finishEncode()
{
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
    m_thread = nullptr;

    QFile file(m_model.filePath());
    file.open(QFile::ReadWrite);
    m_encoder->result(file);
    m_encoder->deleteLater();
    m_encoder = nullptr;
    emit completeFileSave(m_model.filePath());
}
