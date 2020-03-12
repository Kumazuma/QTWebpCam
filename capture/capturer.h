#ifndef CAPTURER_H
#define CAPTURER_H

#include <QObject>
#include <QDesktopWidget>
#include <QPixmap>
#include <QElapsedTimer>
#include <QThread>
#include <QScreen>


struct Capturer : public QObject
{
    Q_OBJECT
protected:
    Capturer(const int fps = 1, QObject* parent = nullptr);
    const int m_fps;
    bool m_isKeepGoing = true;
    uint64_t m_lastCaptureTick;
    QElapsedTimer m_timer;
public:
    virtual bool valid() =0;
    virtual QImage capture()=0;
    virtual ~Capturer(){}
    static std::unique_ptr<Capturer> CreateCapturer(const QRect& captureRect, const int fps = 1, QObject* parent = nullptr);
public slots:
    void startCapture();
    void endCapture();
    void process();
signals:
    void takeImage(QImage img, int duration);
};
#ifdef Q_OS_WIN
#include <windows.h>
#include <DXGITYPE.h>
#include <DXGI1_2.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi.h>
#include <Wincodec.h>
#include <vector>
#include <memory>
class DXGICapturer:public Capturer
{
    Q_OBJECT
private:
    const QRect m_captureRect;
    const int m_fps;
    ID3D11Device *device=nullptr;
    ID3D11DeviceContext *device_context=nullptr;
    IDXGIOutputDuplication *output_duplication=nullptr;
    IDXGIFactory1 *dxgi_factory = nullptr;
    ID3D11Texture2D *capture_texture=nullptr;
    ID3D11Texture2D *region_copy_texture=nullptr;
    IDXGISurface *region_copy_surface=nullptr;
    uint64_t m_lastCaptureTick;
    HWND capture_window;
private:
    static void freeMemory(void*);
public:
    DXGICapturer(const QRect& captureRect, const int fps = 1, QObject* parent = nullptr);
    ~DXGICapturer();
    QImage capture() override;
    bool valid() override;
};
#endif
class QtCapturer: public Capturer
{
    Q_OBJECT
private:
    const QRect m_captureRect;

    QThread * m_thread;
    QDesktopWidget m_desktop;
    QScreen* m_screen;
public:
    QtCapturer(const QRect& captureRect, const int fps = 1, QObject* parent = nullptr);
    ~QtCapturer();
    QImage capture() override;
    bool valid() override{return true;}
};
#endif // CAPTURER_H
