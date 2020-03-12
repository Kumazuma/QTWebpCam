#include<Qt>
#include <QDebug>
#include "capturer.h"
#ifdef Q_OS_WIN
#include <utilapiset.h>
#include <QGuiApplication>
#include <QWindow>

template<typename T>
inline void safe_release(T*& r)
{
    if(r)
        r->Release();
    r = nullptr;
}
void DXGICapturer::freeMemory(void * ptr)
{
    delete[] ptr;
}

DXGICapturer::DXGICapturer(const QRect& captureRect, const int fps, QObject* parent):
    Capturer(fps, parent),
    m_captureRect(captureRect),
    m_fps(fps),
    m_lastCaptureTick(0)
{
    HRESULT hr;
    try
    {
        hr = CreateDXGIFactory1(IID_IDXGIFactory1,
                           reinterpret_cast<void**>(&dxgi_factory));
        if (FAILED(hr))
            throw QStringLiteral("Error create dxgi factory: %1").arg(hr);
        D3D_FEATURE_LEVEL supported_feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };
        D3D_FEATURE_LEVEL fl;
        hr = D3D11CreateDevice(nullptr,
                               D3D_DRIVER_TYPE_HARDWARE,
                               nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                               supported_feature_levels,
                               sizeof(supported_feature_levels)/sizeof(*supported_feature_levels),
                               D3D11_SDK_VERSION,
                               &this->device,
                               &fl,
                               &this->device_context);
        if (FAILED(hr))
            throw QStringLiteral("Error create d3d11 device: %1").arg(hr);

        IDXGIAdapter1 *adapter;
        IDXGIOutput *output;
        for (uint adapter_index = 0; dxgi_factory->EnumAdapters1(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND; adapter_index++)
        {
            // enumerate outputs
            for (uint output_index = 0; adapter->EnumOutputs(output_index, &output) != DXGI_ERROR_NOT_FOUND; output_index++)
            {
                DXGI_OUTPUT_DESC output_desc;
                output->GetDesc(&output_desc);
                if (output_desc.AttachedToDesktop)
                {
                    if (output_desc.DesktopCoordinates.left <= this->m_captureRect.left()
                            && output_desc.DesktopCoordinates.right >= this->m_captureRect.right()
                            && output_desc.DesktopCoordinates.top <= this->m_captureRect.top()
                            && output_desc.DesktopCoordinates.bottom >= this->m_captureRect.bottom())
                    {
//                        this->captured_display_left = output_desc.DesktopCoordinates.left;
//                        this->captured_display_right = output_desc.DesktopCoordinates.right;
//                        this->captured_display_bottom = output_desc.DesktopCoordinates.bottom;
//                        this->captured_display_top = output_desc.DesktopCoordinates.top;

                        IDXGIOutput1 *output1 = static_cast<IDXGIOutput1*>(output);
                        hr = output1->DuplicateOutput(static_cast<IUnknown *>(this->device), &this->output_duplication);
                        if (FAILED(hr))
                            throw QStringLiteral("Output Duplication Failed: %1").arg(hr);
                        qDebug()<<"complete";
                        return;
                    }
                }
                output->Release();
            }
            adapter->Release();
        }
    }
    catch(QString ex)
    {
        qDebug()<<ex;
        safe_release(this->dxgi_factory);
        safe_release(this->device);
        safe_release(this->device_context);
        safe_release(this->output_duplication);
    }
}

DXGICapturer::~DXGICapturer()
{
    safe_release(this->dxgi_factory);
    safe_release(this->device);
    safe_release(this->device_context);
    safe_release(this->output_duplication);
    safe_release(this->region_copy_surface);
    safe_release(this->region_copy_texture);
}

QImage DXGICapturer::capture()
{
    DXGI_OUTDUPL_FRAME_INFO capture_frame_info;
    IDXGIResource *resource;
    HRESULT hr = S_OK;

    hr = this->output_duplication->AcquireNextFrame(0, &capture_frame_info, &resource);
    if(FAILED(hr))
    {
        qDebug()<<"Error: no new frames";
        return QImage();
    }
    resource->QueryInterface(IID_ID3D11Texture2D, reinterpret_cast<void**>(&this->capture_texture));
    resource->Release();
    if(!this->capture_texture)
    {
        qDebug()<< "error - capture_texture";
        return QImage();
    }

    D3D11_TEXTURE2D_DESC capture_texture_desc;
    D3D11_TEXTURE2D_DESC region_texture_desc;
    this->capture_texture->GetDesc(&capture_texture_desc);
    memset(&region_texture_desc, 0, sizeof(region_texture_desc));
    region_texture_desc.Width = static_cast<uint>(this->m_captureRect.width());
    region_texture_desc.Height = static_cast<uint>(this->m_captureRect.height());
    region_texture_desc.MipLevels = 1;
    region_texture_desc.ArraySize = 1;
    region_texture_desc.SampleDesc.Count = 1;
    region_texture_desc.SampleDesc.Quality = 0;
    region_texture_desc.Usage = D3D11_USAGE_STAGING;
    region_texture_desc.Format = capture_texture_desc.Format;
    region_texture_desc.BindFlags = 0;
    region_texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    region_texture_desc.MiscFlags = 0;
    hr = this->device->CreateTexture2D(&region_texture_desc, nullptr, &this->region_copy_texture);
    if (FAILED(hr))
    {
        safe_release(this->capture_texture);
        qDebug()<<QStringLiteral("error - CreateTexture2d: %1").arg(hr);
        return QImage();
    }

    D3D11_BOX source_region;
    source_region.left = static_cast<uint>(this->m_captureRect.left());
    source_region.right = static_cast<uint>(this->m_captureRect.right());
    source_region.top = static_cast<uint>(this->m_captureRect.top());
    source_region.bottom = static_cast<uint>(this->m_captureRect.bottom());
    source_region.front = 0;
    source_region.back = 1;
    this->device_context->CopySubresourceRegion(static_cast<ID3D11Resource *>(this->region_copy_texture), 0, 0, 0, 0, static_cast<ID3D11Resource *>(this->capture_texture), 0, &source_region);
    this->region_copy_texture->QueryInterface(IID_IDXGISurface, reinterpret_cast<void**>(&this->region_copy_surface));
    DXGI_MAPPED_RECT rect;
    hr = this->region_copy_surface->Map(&rect, DXGI_MAP_READ);
    if (FAILED(hr))
    {

        this->region_copy_surface->Unmap();
        this->region_copy_surface->Release();
        this->region_copy_texture->Release();
        qDebug()<< QStringLiteral("error - CreateTexture2d: %1").arg(hr);
        //captureStateInit(this->x, this->y, this->w, this->h);
    }
    auto height = this->m_captureRect.height();
    auto width = this->m_captureRect.width();
    unsigned char *dest = new unsigned char[height * width * 4];
    unsigned char *src = rect.pBits;
    int comp = 4;
    memcpy(dest, src, height * comp * width);
    for (int row = 0; row < this->m_captureRect.height(); row++)
    {
        auto d = dest;
        d = d + this->m_captureRect.width() * comp * row;
        memcpy(d, src, static_cast<size_t>(this->m_captureRect.width() * comp));
        src += rect.Pitch;
    }
    this->region_copy_surface->Unmap();
    this->output_duplication->ReleaseFrame();
    safe_release(this->region_copy_surface);
    safe_release(this->region_copy_texture);

    return QImage(dest, m_captureRect.width(), m_captureRect.height(),QImage::Format_RGB32, &DXGICapturer::freeMemory, dest);

}

bool DXGICapturer::valid()
{
    return this->device != nullptr;
}
#endif
