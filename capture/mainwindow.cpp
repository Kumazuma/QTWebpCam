#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QRegion"
#include "algorithm"
#include <QMessageBox>
#include <QStyle>
#include <QMoveEvent>
#ifdef Q_OS_WIN
#include<windows.h>
#endif
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_presenter(new  Presenter(this) )
{
    setWindowFlags(Qt::WindowStaysOnTopHint| Qt::WindowType::Window | Qt::FramelessWindowHint);
    ui->setupUi(this);

    connect(ui->pushButton_3, SIGNAL(released()), this, SLOT(onClickedClose()));
    connect(ui->spinWidth, SIGNAL(valueChanged(int)), this, SLOT(onSpinWidthChange(int)));
    connect(ui->spinHeight, SIGNAL(valueChanged(int)), this, SLOT(onSpinHeightChange(int)));
    connect(ui->spinX, SIGNAL(valueChanged(int)), this, SLOT(onSpinPosXChange(int)));
    connect(ui->spinY, SIGNAL(valueChanged(int)), this, SLOT(onSpinPosYChange(int)));
    connect(m_presenter, SIGNAL(changedRect(const QRect&)), this, SLOT(onChangedRecoredRect(const QRect)));
    connect(m_presenter, SIGNAL(startRecord()), this, SLOT(startRecord()));
    connect(m_presenter, SIGNAL(finishRecord(FileImageStore*)), this, SLOT(showEditWindow(FileImageStore*)));
    connect(ui->btnStart, SIGNAL(released()), m_presenter, SLOT(startCapture()));
    connect(ui->btnStop, SIGNAL(released()), m_presenter, SLOT(stopCapture()));
    connect(ui->cbFps, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onFpsChanged(const QString&)));
    connect(m_presenter, SIGNAL(updateStore(int,int)), this, SLOT(onUpdateStore(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    QRegion region(this->rect());
    region -= QRegion(ui->verticalSpacer->geometry());
    setMask(region);
    auto rt = ui->verticalSpacer->geometry().translated(this->pos());
    m_presenter->setRecoredRect(rt);

}
void MainWindow::showEvent(QShowEvent* event)
{
    QRegion region(this->rect());
    region -= QRegion(ui->verticalSpacer->geometry());
    setMask(region);
    //
    QPoint pt = this->pos();
    QRect rect = ui->verticalSpacer->geometry();
    auto ss = rect.size();
    pt += pt + rect.topLeft();
    rect = QRect(pt.x(),pt.y(),ss.width(),ss.height());
    auto width = ui->verticalSpacer->geometry().width();
    ui->spinWidth->setValue(width);
    m_presenter->setRecoredRect(rect);

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void* msg, long *result)
{
    MSG& winMsg = *(MSG*)msg;
    switch(winMsg.message)
    {
        case WM_NCHITTEST:
        if(!m_isResizable)
            break;
        int x = LOWORD(winMsg.lParam);
        int y = HIWORD(winMsg.lParam);
        POINT pt{x,y};
        ScreenToClient((HWND)this->winId(), &pt);
        QPoint point(pt.x,pt.y);
        auto t = rect();
        t.setX(9);
        t.setY(9);
        t.setWidth(t.width() - 18);
        t.setHeight(t.height() - 18);

        if(point.x() <= t.left() && point.y() <= t.top())
            *result = HTTOPLEFT;
        else if(point.x() < t.left() && point.y() < t.bottom())
            *result = HTLEFT;
        else if(point.x() <= t.left() && point.y() >= t.bottom() )
            *result = HTBOTTOMLEFT;
        else if(point.x() >= t.right() && point.y() <= t.top())
            *result = HTTOPRIGHT;
        else if(point.x() > t.right() && point.y() < t.bottom() )
            *result = HTRIGHT;
        else if(point.x() >= t.right() && point.y() >= t.bottom())
            *result = HTBOTTOMRIGHT;
        else if(point.y() < t.top())
            *result = HTTOP;
        else if(point.y() > t.bottom())
            *result = HTBOTTOM;
        else
        {
            auto topRect = ui->captionArea->geometry();
            //bool cond1 = point.x() < 30;
            bool cond = topRect.contains(point);
            if(cond)
                *result = HTCAPTION;
            else
                break;

        }
        return true;
    }

    return QMainWindow::nativeEvent(eventType, result, result);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    this->deleteLater();
}
void MainWindow::onClickedClose()
{
    this->close();
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    auto spacer = ui->verticalSpacer;
    auto rect = spacer->geometry();
    auto pos = event->pos();
    auto rect2 = rect.translated(pos);
    m_presenter->setRecoredRect(rect2);
    QRegion region(this->rect());
    region -= QRegion(spacer->geometry());
    QMainWindow::moveEvent(event);
}
void MainWindow::onSpinWidthChange(int  val)
{
    auto rect = m_presenter->recoredRect();
    rect.setWidth(val);
    auto winRect = this->rect();
    auto gap = winRect.width() - ui->verticalSpacer->geometry().width();
    winRect.setWidth(rect.width() + gap);
    this->resize(winRect.size());
}
void MainWindow::onSpinHeightChange(int val)
{
    auto rect = m_presenter->recoredRect();
    rect.setHeight(val);
    auto winRect = this->rect();
    auto gap = winRect.height() - ui->verticalSpacer->geometry().height();
    winRect.setHeight(rect.height() + gap);
    this->resize(winRect.size());
}
void MainWindow::onSpinPosXChange(int val)
{
    auto rect = m_presenter->recoredRect();
    auto delta = val - rect.x();
    auto pos = this->pos();
    pos.setX(pos.x() + delta);
    this->move(pos);
}
void MainWindow::onSpinPosYChange(int val)
{
    auto rect = m_presenter->recoredRect();
    auto delta = val - rect.y();
    auto pos = this->pos();
    pos.setY(pos.y() + delta);
    this->move(pos);
}

void MainWindow::onChangedRecoredRect(const QRect &rect)
{
    ui->spinX->setValue(rect.x());
    ui->spinY->setValue(rect.y());
    ui->spinWidth->setValue(rect.width());
    ui->spinHeight->setValue(rect.height());
}

void MainWindow::onFpsChanged(const QString &text)
{
    m_presenter->setFps( text.toInt());
}
#include "../edit/editwindow.h"
void MainWindow::showEditWindow(FileImageStore *store)
{
    EditWindow * window = new EditWindow(store);
    window->show();
    this->close();
}

void MainWindow::startRecord()
{
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    m_isResizable = false;
    setFixedSize(this->geometry().size());
}

void MainWindow::onUpdateStore(int count, int size)
{
    QString formattedString = "%1장 %2MB";
    ui->lblState->setText(formattedString.arg(count).arg(size/(1024 * 1024)));
}
