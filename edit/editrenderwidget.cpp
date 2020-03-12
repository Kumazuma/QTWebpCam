#include "editrenderwidget.h"
#include <QMouseEvent>
#include <QScrollBar>
EditRenderWidget::EditRenderWidget(EditPresenter& presenter, QWidget* parent):
    QGraphicsView(parent),
    m_presenter(presenter),
    m_scale(1.0),
    m_behavior(new DefaultBehavior())
{
    setMouseTracking(true);
    connect(&m_presenter, SIGNAL(currentImageFrame(ImageFrame)), this, SLOT(selectedFrame(const ImageFrame &)));
    connect(&m_presenter, &EditPresenter::updateImageStore, this, &EditRenderWidget::reloadImageStore);
    m_cropRect.setX(0);
    m_cropRect.setY(0);
    m_cropRect.setSize(m_presenter.imageStore().imageSize());

}

EditRenderWidget::~EditRenderWidget()
{
#ifndef QT_NO_DEBUG
    qDebug()<<"method: "<< __FUNCTION__;
#endif

}

void EditRenderWidget::setScale(qreal val)
{
    m_scale = val;
    setTransform(QTransform::fromScale(m_scale,m_scale));
}

void EditRenderWidget::setCropRect(const QRect &rect)
{
    if(rect == m_cropRect)
        return;
    m_cropRect = rect;

    scene()->removeItem(m_pathItem);
    QPainterPath path;
    path.addRect(QRect(QPoint(0,0), imageSize()));
    path.addRect(m_cropRect);
    QBrush brush(QColor::fromRgba(qRgba(0,0,0,128)));
    QPen pen(Qt::PenStyle::DotLine);
    pen.setColor(QColor(Qt::GlobalColor::white));
    m_pathItem = scene()->addPath(path,pen,brush);
    emit cropRectUpdate(m_cropRect);
}

void EditRenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto scene = this->scene();
    if(!scene)
        return;
    auto behavior = m_behavior->mouseMoveEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
}

void EditRenderWidget::mousePressEvent(QMouseEvent *event)
{
    auto scene = this->scene();
    if(!scene)
        return;
    auto behavior = m_behavior->mousePressEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
}

void EditRenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    auto scene = this->scene();
    if(!scene)
        return;
    auto behavior = m_behavior->mouseReleaseEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
}

void EditRenderWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QGraphicsView::dragEnterEvent(event);
}

void EditRenderWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    QGraphicsView::dragLeaveEvent(event);
}

void EditRenderWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
}

void EditRenderWidget::wheelEvent(QWheelEvent *event)
{
    auto behavior = m_behavior->wheelEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
//    if(m_isPressCirl)
//    {
//        int d = event->delta();
//        m_scale += 0.25 * d / abs(d);
//        setTransform(QTransform::fromScale(m_scale,m_scale));
//    }
}

void EditRenderWidget::keyPressEvent(QKeyEvent *event)
{
    auto behavior = m_behavior->keyPressEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
}

void EditRenderWidget::keyReleaseEvent(QKeyEvent *event)
{
    auto behavior = m_behavior->keyReleaseEvent(*this, *event);
    if(behavior)
        m_behavior.swap(*behavior);
}

void EditRenderWidget::resizeEvent(QResizeEvent *event)
{
    auto size = event->size();
    verticalScrollBar()->setPageStep(size.height() / 10);
    horizontalScrollBar()->setPageStep(size.width() / 10);
}

void EditRenderWidget::selectedFrame(const ImageFrame &frame)
{
    qDebug()<<__PRETTY_FUNCTION__;
    if(m_cropRect.x() < 0)
        m_cropRect.setX(0);
    if(m_cropRect.y() < 0)
        m_cropRect.setY(0);
    auto imgSize = m_presenter.imageStore().imageSize();
    if(m_cropRect.width() + m_cropRect.x() > imgSize.width())
    {
        m_cropRect.setWidth(imgSize.width() - m_cropRect.x());
    }
    if(m_cropRect.height() + m_cropRect.y() > imgSize.height())
    {
        m_cropRect.setHeight(imgSize.height() - m_cropRect.y());
    }
    auto scene = this->scene();
    if(scene != nullptr)
    {
        delete scene;
    }
    auto image = m_presenter.getImageFromImageFrame(frame);

    scene = new QGraphicsScene(this);
    auto pixmap = QPixmap::fromImage(image);

    QPainterPath path;
    path.addRect(pixmap.rect());
    path.addRect(m_cropRect);
    QBrush brush(QColor::fromRgba(qRgba(0,0,0,128)));
    QPen pen(Qt::PenStyle::DotLine);
    pen.setColor(QColor(Qt::GlobalColor::white));
    scene->addPixmap(pixmap);
    m_pathItem = scene->addPath(path,pen,brush);

    this->setScene(scene);
}

void EditRenderWidget::reloadImageStore()
{
    this->setScene(nullptr);
    m_cropRect.setX(0);
    m_cropRect.setY(0);
    m_cropRect.setSize(m_presenter.imageStore().imageSize());

    emit cropRectUpdate(m_cropRect);
}
