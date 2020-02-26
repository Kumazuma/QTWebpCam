#include "capturer.h"
#include <QDebug>
#include <QScreen>
Capturer::Capturer(const QRect& captureRect, const int fps, QObject* parent):
    QObject(parent),
    m_captureRect(captureRect),
    m_fps(fps),
    m_lastCaptureTick(0)
{

}
void Capturer::startCapture()
{
    m_timer.start();
    emit keepProcess();
}
void Capturer::endCapture()
{
    m_isKeepGoing = false;
}

QPixmap Capturer::capture()
{
    auto& rect = m_captureRect;
    QDesktopWidget desktop;

    return QPixmap::grabWindow(desktop.winId(),
                          m_captureRect.x(),
                          m_captureRect.y(),
                          m_captureRect.width(),
                               m_captureRect.height());
}

void Capturer::process()
{
    auto now = m_timer.elapsed();
    auto delta = now - m_lastCaptureTick;
    auto d = 1000 / m_fps;
    if(delta >= d)
    {
        m_lastCaptureTick = now;
        auto res = capture();
        auto img = res.toImage();
        emit takeImage(img,delta);
    }
    if(m_isKeepGoing)
    {
        emit keepProcess();
    }
    else
    {
        emit finished();
    }
}

void Capturer::stop()
{
    m_isKeepGoing = false;
    emit finished();
}

