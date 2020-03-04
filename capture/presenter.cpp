#include "presenter.h"
#include <iostream>
Presenter::Presenter(QObject * parent):
    QObject(parent),
    m_thread(this)
{

}

Presenter::~Presenter()
{
    if(m_timer!= nullptr && m_timer->isActive())
    {
        m_timer->stop();
    }
    if(!m_thread.isFinished())
    {
        m_thread.quit();
        m_thread.wait();
    }
    if(m_builder != nullptr)
    {
        delete m_builder;
    }
}

QRect Presenter::recoredRect() const
{
    return m_model.recoredRect();
}

int Presenter::fps() const
{
    return m_model.fps();
}
void Presenter::setFps(int val)
{
    m_model.fps(val);
}
void Presenter::setRecoredRect(const QRect& rect)
{
    if(rect == m_model.recoredRect())
            return;
    m_model.recoredRect(rect);
    emit changedRect(m_model.recoredRect());
}
#include <QAbstractEventDispatcher>
void Presenter::startCapture()
{
    if(m_capture != nullptr)
        return;
    emit startRecord();

    m_capture = new Capturer(m_model.recoredRect(), m_model.fps(), this);
    m_builder = new MemoryMapStoreBuilder(m_model.recoredRect().size(), nullptr);
    m_timer = new QTimer(this);
    m_builder->moveToThread(&m_thread);
    //슬롯-시그널 정의
    QObject::connect(m_timer, &QTimer::timeout, m_capture, &Capturer::process);
    void(MemoryMapStoreBuilder::*pushBack)(QImage, int) = &MemoryMapStoreBuilder::pushBack;
    QObject::connect(m_capture, &Capturer::takeImage, m_builder, pushBack);
    QObject::connect(&m_thread, &QThread::finished, this, &Presenter::onExitThread);
    QObject::connect(m_builder, SIGNAL(updateStore(int,int)), this, SLOT(onUpdateStore(int,int)));

    m_thread.start();
    m_capture->startCapture();
    m_timer->start(1);
}

void Presenter::stopCapture()
{
    m_capture->stop();
    m_timer->stop();
    m_thread.quit();
}

void Presenter::onExitThread()
{
    auto store = m_builder->buildStore();
    emit finishRecord(store);
}

void Presenter::onUpdateStore(int count, int size)
{
    emit updateStore(count, size);
}
