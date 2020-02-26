#ifndef CAPTUREPREVIEWWINDOW_H
#define CAPTUREPREVIEWWINDOW_H

#include <QMainWindow>
#include <QPixmap>
namespace Ui {
class CapturePreviewWindow;
}

class CapturePreviewWindow : public QMainWindow
{
    Q_OBJECT
private:
    QImage m_screenshot;
public:
    explicit CapturePreviewWindow(QWidget *parent = nullptr,const QImage& screenshot = QImage());
    ~CapturePreviewWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::CapturePreviewWindow *ui;
};

#endif // CAPTUREPREVIEWWINDOW_H
