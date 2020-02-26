#include "presenter.h"
#include <iostream>
Presenter::Presenter(QObject * parent):
    QObject(parent)
{

}

Presenter::~Presenter()
{
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
    if(m_thread.isRunning())
        return;
    m_capture = new Capturer(m_model.recoredRect(), m_model.fps(), this);
    m_builder = new FileImageStoreBuilder(m_model.recoredRect().size(), this);
    m_timer = new QTimer(this);
    //스레드 이동
    m_capture->moveToThread(&m_thread);
    m_builder->moveToThread(&m_thread);
    m_timer->moveToThread(&m_thread);
    //슬롯-시그널 정의
    QObject::connect(&m_thread, &QThread::started, m_capture, &Capturer::startCapture);
    QObject::connect(&m_thread, &QThread::finished, m_timer, &QTimer::stop );
    QObject::connect(&m_thread, &QThread::finished, m_capture, &Capturer::stop );
    QObject::connect(m_timer, &QTimer::timeout, m_capture, &Capturer::process);
    QObject::connect(m_capture, &Capturer::takeImage, m_builder, &FileImageStoreBuilder::pushBack);
    QObject::connect(m_capture, &Capturer::finished, this, &Presenter::onExitThread);
    m_timer->start(2);
    m_thread.start();
}

void Presenter::stopCapture()
{
    m_thread.quit();
}

void Presenter::onExitThread()
{
    auto store = m_builder->buildStore();
    emit finishRecord(store);
}
