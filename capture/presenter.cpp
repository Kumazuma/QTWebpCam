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

void Presenter::startCapture()
{
    if(m_capture != nullptr)
        return;
    emit startRecord();

    m_capture = new Capturer(m_model.recoredRect(), m_model.fps(), this);
    m_builder = new FileImageStoreBuilder(m_model.recoredRect().size(), nullptr);
    m_timer = new QTimer(this);
    m_builder->moveToThread(&m_thread);
    m_capture->moveToThread(&m_thread);
    //슬롯-시그널 정의
    QObject::connect(m_timer, &QTimer::timeout, m_capture, &Capturer::process);
    QObject::connect(m_capture, &Capturer::takeImage, m_builder, &FileImageStoreBuilder::pushBack);
    QObject::connect(&m_thread, &QThread::finished, this, &Presenter::onExitThread);
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
