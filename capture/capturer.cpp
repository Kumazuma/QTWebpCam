#include "capturer.h"
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
Capturer::Capturer(const int fps, QObject* parent):
    QObject(parent),
    m_fps(fps),
    m_lastCaptureTick(0)
{

}
std::unique_ptr<Capturer> Capturer::CreateCapturer(const QRect &captureRect, const int fps, QObject *parent)
{
    Capturer * res = nullptr;
#ifdef Q_OS_WIN
    res = new DXGICapturer(captureRect, fps, parent);
    if(res->valid())
        return std::unique_ptr<Capturer>(res);
#endif
    res = new QtCapturer(captureRect, fps, parent);
    if(res->valid())
        return std::unique_ptr<Capturer>(res);
    return nullptr;
}

void Capturer::startCapture()
{
    m_timer.start();
    m_lastCaptureTick = m_timer.elapsed();
}
void Capturer::endCapture()
{
    m_isKeepGoing = false;
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
        emit takeImage(res,delta);
    }
}
QtCapturer::QtCapturer(const QRect &captureRect, const int fps, QObject *parent):
    Capturer(fps, parent),
    m_captureRect(captureRect),
    m_screen(QGuiApplication::primaryScreen())
{
}

QtCapturer::~QtCapturer()
{
#ifndef QT_NO_DEBUG
    qDebug()<<"method: "<< __FUNCTION__;
#endif
}


QImage QtCapturer::capture()
{
    //QDesktopWidget desktop;

    return m_screen->grabWindow(m_desktop.winId(),
                          m_captureRect.x(),
                          m_captureRect.y(),
                          m_captureRect.width(),
                            m_captureRect.height()).toImage();
}



