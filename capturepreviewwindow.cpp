#include "capturepreviewwindow.h"
#include "ui_capturepreviewwindow.h"
#include <QPainter>
#include <QDebug>
CapturePreviewWindow::CapturePreviewWindow(QWidget *parent, const QImage& screenshot) :
    QMainWindow(parent),
    ui(new Ui::CapturePreviewWindow),
    m_screenshot(screenshot)
{
    ui->setupUi(this);
}

CapturePreviewWindow::~CapturePreviewWindow()
{
    delete ui;
}

void CapturePreviewWindow::paintEvent(QPaintEvent *event)
{
    if(m_screenshot.isNull())
    {
        qDebug()<<"this image is null";
        return;
    }

    QPainter painter(this);
    painter.drawImage(QPoint(0,0), m_screenshot);
}
