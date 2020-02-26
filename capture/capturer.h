#ifndef CAPTURER_H
#define CAPTURER_H

#include <QObject>
#include <QDesktopWidget>
#include <QPixmap>
#include <QElapsedTimer>
#include <QThread>
class Capturer: public QObject
{
    Q_OBJECT
private:
    const QRect m_captureRect;
    const int m_fps;
    bool m_isKeepGoing = true;
    uint64_t m_lastCaptureTick;
    QElapsedTimer m_timer;
    QThread * m_thread;
public:
    Capturer(const QRect& captureRect, const int fps = 1, QObject* parent = nullptr);

    QPixmap capture();
public slots:
    void startCapture();
    void endCapture();
    void process();
    void stop();
signals:
    void requstStart();
    void finished();
    void keepProcess();
    void takeImage(QImage img, uint64_t duration);
};
#endif // CAPTURER_H
