#include "editrenderwidget.h"
#include <QMouseEvent>
EditRenderWidget::EditRenderWidget(const EditPresenter& presenter, QWidget* parent):
    QGraphicsView(parent),
    m_presenter(presenter)
{
    setMouseTracking(true);
    connect(&m_presenter, &EditPresenter::currentImageFrame, this, &EditRenderWidget::selectedFrame);
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

std::array<qint8, 2> EditRenderWidget::calcSizingDirections(const QPointF &point)
{
    std::array<qint8, 2> res = {0,0};
    QRectF t;
    t.setX(m_cropRect.x() - 9);
    t.setY(m_cropRect.y() - 9);
    t.setWidth(m_cropRect.width() + 18);
    t.setHeight(m_cropRect.height() + 18);

    if(t.contains(point))
    {
        t.setX(m_cropRect.x() + 9);
        t.setY(m_cropRect.y() + 9);
        t.setWidth(m_cropRect.width() - 18);
        t.setHeight(m_cropRect.height() - 18);
        if(point.x() <= t.left())
            res[0] = -1;
        if(point.x() >= t.right())
            res[0] = 1;
        if(point.y() <= t.top())
            res[1] = -1;
        if(point.y() >= t.bottom())
            res[1] = 1;
    }
    return res;
}

QCursor EditRenderWidget::getSizingDirectionCursor(const std::array<qint8, 2> &axis)
{
    Qt::CursorShape shape = Qt::CursorShape::ArrowCursor;
    if(axis[0] != 0 || axis[1] != 0)
    {
        if(axis[0] == axis[1])
        {
            shape = Qt::SizeFDiagCursor;
        }
        if(axis[0] != axis[1])
        {
            shape = Qt::SizeBDiagCursor;
        }
        if(axis[1] == 0 && axis[0] != 0)
        {
            shape = Qt::SizeHorCursor;
        }
        if(axis[1] != 0 && axis[0] == 0)
        {
            shape = Qt::SizeVerCursor;
        }
    }
    return shape;
}

void EditRenderWidget::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    auto scene = this->scene();
    if(!scene)
        return;
    auto point = mapToScene(event->pos());
    if(!m_sizingAxis)
    {

        auto direct = calcSizingDirections(point);
        setCursor(QCursor(getSizingDirectionCursor(direct)));
        return;
    }
    auto delta = point - m_prevPoint;
    m_prevPoint = point;
    if(m_sizingAxis->at(0) == -1)
        m_cropRect.translate(delta.x(), 0);
    if(m_sizingAxis->at(1) == -1)
        m_cropRect.translate(0, delta.y());
    m_cropRect.setWidth(m_cropRect.width() +m_sizingAxis->at(0) * delta.x());
    m_cropRect.setHeight(m_cropRect.height() +m_sizingAxis->at(1) * delta.y());

    auto imgSize = m_presenter.imageStore().imageSize();
    if(m_cropRect.x() < 0)
        m_cropRect.setX(0);
    if(m_cropRect.y() < 0)
        m_cropRect.setY(0);
    if(m_cropRect.x() >= imgSize.width())
        m_cropRect.setX(imgSize.width() - 10);
    if(m_cropRect.y() >= imgSize.height())
        m_cropRect.setY(imgSize.height() - 10);
    if(m_cropRect.width() <= 0)
        m_cropRect.setWidth(10);
    if(m_cropRect.height() <= 0)
        m_cropRect.setHeight(10);

    this->scene()->removeItem(m_pathItem);
    QPainterPath path;
    path.addRect(QRect(QPoint(0,0), m_presenter.imageStore().imageSize()));
    path.addRect(m_cropRect);
    QBrush brush(QColor::fromRgba(qRgba(0,0,0,128)));
    QPen pen(Qt::PenStyle::DotLine);
    pen.setColor(QColor(Qt::GlobalColor::white));
    m_pathItem = scene->addPath(path,pen,brush);
}

void EditRenderWidget::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    auto scene = this->scene();
    if(!scene)
        return;
    auto pos = mapToScene(event->pos());
    auto sizing =  calcSizingDirections(pos);
    if(sizing[0] == 0 && sizing[1] == 0)
        return;
    m_sizingAxis = sizing;
    m_prevPoint = pos;
    setCursor(getSizingDirectionCursor(sizing));
}

void EditRenderWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    auto scene = this->scene();
    if(scene == nullptr)
        return;
    if(!m_sizingAxis)
        return;

    setCursor(Qt::ArrowCursor);
    if((*m_sizingAxis)[0] == 0 && (*m_sizingAxis)[1] == 0)
    {
        m_sizingAxis = std::nullopt;
        return;
    }
    auto point = mapToScene(event->pos());
    auto delta = point - m_prevPoint;
    m_prevPoint = point;
    if(m_sizingAxis->at(0) == -1)
        m_cropRect.translate(delta.x(), 0);
    if(m_sizingAxis->at(1) == -1)
        m_cropRect.translate(0, delta.y());
    m_cropRect.setWidth(m_cropRect.width() +m_sizingAxis->at(0) * delta.x());
    m_cropRect.setHeight(m_cropRect.height() +m_sizingAxis->at(1) * delta.y());
    auto imgSize = m_presenter.imageStore().imageSize();
    if(m_cropRect.x() < 0)
        m_cropRect.setX(0);
    if(m_cropRect.y() < 0)
        m_cropRect.setY(0);
    if(m_cropRect.x() >= imgSize.width())
        m_cropRect.setX(imgSize.width() - 10);
    if(m_cropRect.y() >= imgSize.height())
        m_cropRect.setY(imgSize.height() - 10);
    if(m_cropRect.width() <= 0)
        m_cropRect.setWidth(10);
    if(m_cropRect.height() <= 0)
        m_cropRect.setHeight(10);
    if(m_cropRect.width() + m_cropRect.x() > imgSize.width())
    {
        m_cropRect.setWidth(imgSize.width() - m_cropRect.x());
    }
    if(m_cropRect.height() + m_cropRect.y() > imgSize.height())
    {
        m_cropRect.setHeight(imgSize.height() - m_cropRect.y());
    }

    this->scene()->removeItem(m_pathItem);
    QPainterPath path;
    path.addRect(QRect(QPoint(0,0), m_presenter.imageStore().imageSize()));
    path.addRect(m_cropRect);
    QBrush brush(QColor::fromRgba(qRgba(0,0,0,128)));
    QPen pen(Qt::PenStyle::DotLine);
    pen.setColor(QColor(Qt::GlobalColor::white));

    m_pathItem = scene->addPath(path,pen,brush);
    m_sizingAxis = std::nullopt;

    emit cropRect(m_cropRect);
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

void EditRenderWidget::selectedFrame(const ImageFrame &frame)
{
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

    auto image = m_presenter.getImageFromImageFrame(frame);
    auto scene = new QGraphicsScene(this);
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

    emit cropRect(m_cropRect);
}
