#include "renderwidgetsbehavior.h"
#include "editrenderwidget.h"


DefaultBehavior::DefaultBehavior()
{

}

DefaultBehavior::~DefaultBehavior()
{

}


ReturnType DefaultBehavior::mouseMoveEvent(EditRenderWidget & widget, const QMouseEvent & event)
{
    auto point = widget.mapToScene(event.pos());
    if(!m_sizingAxis)
    {

        auto direct = calcSizingDirections(widget.cropRect(), point);
        widget.setCursor(QCursor(getSizingDirectionCursor(direct)));
        return std::nullopt;
    }
    auto delta = (point - m_prevPoint)/widget.scale();
    qDebug()<<"delta is " << delta;

    auto temp = widget.cropRect();
    auto axis = *m_sizingAxis;
    if(axis[0] != 0 &&  abs(delta.x()) < 1 )
    {
        return std::nullopt;
    }
    if(axis[1] != 0 &&  abs(delta.y()) < 1 )
    {
        return std::nullopt;
    }
    m_prevPoint = point;
    double t = 0.0;
    auto imgSize = widget.imageSize();
    t = temp.x() + delta.x();
    if(axis[0] == -1 && t >= 0)
    {
        temp.setX(t);
        //temp.setWidth(temp.width() + delta.x());
    }
    t = temp.width() + delta.x();
    if(axis[0] == 1 && t >= 10 && t <= imgSize.width())
    {
        temp.setWidth(t);
    }
    t = temp.y() + delta.y();
    if(axis[1] == -1 && t >= 0)
    {
        temp.setY(t);
        //temp.setHeight(temp.height() + delta.y());
    }
    t = temp.height() + delta.y();
    if(axis[1] == 1 && t >= 10 && t <= imgSize.height())
    {
        temp.setHeight(t);
    }
    widget.setCropRect(temp);
    return std::nullopt;
}

ReturnType DefaultBehavior::mousePressEvent(EditRenderWidget & widget, const QMouseEvent& event)
{
    auto pos = widget.mapToScene(event.pos());
    auto sizing =  calcSizingDirections(widget.cropRect(), pos);
    if(sizing[0] == 0 && sizing[1] == 0)
        return std::nullopt;
    m_sizingAxis = sizing;
    m_prevPoint = pos;
    widget.setCursor(getSizingDirectionCursor(sizing));
    return std::nullopt;
}

ReturnType DefaultBehavior::mouseReleaseEvent(EditRenderWidget& widget, const QMouseEvent& event)
{
    auto point = widget.mapToScene(event.pos());
    if(!m_sizingAxis)
    {

        auto direct = calcSizingDirections(widget.cropRect(), point);
        widget.setCursor(QCursor(getSizingDirectionCursor(direct)));
        return std::nullopt;
    }
    auto delta = (point - m_prevPoint)/widget.scale();
    qDebug()<<"delta is " << delta;

    auto temp = widget.cropRect();
    auto axis = *m_sizingAxis;
    m_sizingAxis = std::nullopt;
    if(axis[0] != 0 &&  abs(delta.x()) < 1 )
    {
        return std::nullopt;
    }
    if(axis[1] != 0 &&  abs(delta.y()) < 1 )
    {
        return std::nullopt;
    }
    m_prevPoint = point;
    double t = 0.0;
    auto imgSize = widget.imageSize();
    t = temp.x() + delta.x();
    if(axis[0] == -1 && t >= 0)
    {
        temp.setX(t);
        //temp.setWidth(temp.width() + delta.x());
    }
    t = temp.width() + delta.x();
    if(axis[0] == 1 && t >= 10 && t <= imgSize.width())
    {
        temp.setWidth(t);
    }
    t = temp.y() + delta.y();
    if(axis[1] == -1 && t >= 0)
    {
        temp.setY(t);
        //temp.setHeight(temp.height() + delta.y());
    }
    t = temp.height() + delta.y();
    if(axis[1] == 1 && t >= 10 && t <= imgSize.height())
    {
        temp.setHeight(t);
    }
    widget.setCropRect(temp);
    return std::nullopt;
}
ReturnType DefaultBehavior::wheelEvent(EditRenderWidget & widget, const QWheelEvent & event)
{
    return std::nullopt;
}

ReturnType DefaultBehavior::keyPressEvent(EditRenderWidget & widget, const QKeyEvent & event)
{
    switch(event.key())
    {
    case Qt::Key_Control:
    return std::unique_ptr<RenderWidgetsBehavior>(new ScaleBehavior());
    case Qt::Key_Space:
    return std::unique_ptr<RenderWidgetsBehavior>(new GrabMoveBehavior(widget));
//    case Qt::Key_Shift:
//    return std::unique_ptr<RenderWidgetsBehavior>(new HScrollBehavior());
    }
    return std::nullopt;
}
QCursor DefaultBehavior::getSizingDirectionCursor(const std::array<qint8, 2> &axis)
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
std::array<qint8, 2> DefaultBehavior::calcSizingDirections(const QRect& cropRect, const QPointF &point)
{
    std::array<qint8, 2> res = {0,0};
    QRectF t;
    t.setX(cropRect.x() - 9);
    t.setY(cropRect.y() - 9);
    t.setWidth(cropRect.width() + 18);
    t.setHeight(cropRect.height() + 18);

    if(t.contains(point))
    {
        t.setX(cropRect.x() + 9);
        t.setY(cropRect.y() + 9);
        t.setWidth(cropRect.width() - 18);
        t.setHeight(cropRect.height() - 18);
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

ReturnType ScaleBehavior::wheelEvent(EditRenderWidget & widget, const QWheelEvent & event)
{
    auto scale = widget.scale();
    int d = event.delta();
    scale += 0.25 * d / abs(d);
    widget.setScale(scale);
    return std::nullopt;
}


ReturnType ScaleBehavior::keyReleaseEvent(EditRenderWidget & widget, const QKeyEvent & event)
{
    switch(event.key())
    {
    case Qt::Key_Control:
        return std::unique_ptr<RenderWidgetsBehavior>( new DefaultBehavior());
    }
    return std::nullopt;
}
#include<QApplication>
GrabMoveBehavior::GrabMoveBehavior(EditRenderWidget & widget)
{
    m_prev = widget.mapFromGlobal(QCursor::pos());
}
#include<QScrollBar>
ReturnType GrabMoveBehavior::mouseMoveEvent(EditRenderWidget &widget, const QMouseEvent & event)
{
    auto now = event.pos();
    auto delta = (now - m_prev)/widget.scale();
    if(delta.x() == 0 && delta.y() == 0)
        return std::nullopt;
    m_prev = now;

    auto y =  widget.verticalScrollBar()->value() + widget.height()/2;
    auto x = widget.horizontalScrollBar()->value() + widget.width()/2;
    //widget.centerOn(,);
    //delta += //QPoint(t.dx(), t.dy());
    //t = QTransform::fromTranslate(delta.x(), delta.y()).scale(widget.scale(),widget.scale());
    //qDebug()<<t;
    //widget.setTransform(t,true);

    return std::nullopt;
}

ReturnType GrabMoveBehavior::keyReleaseEvent(EditRenderWidget &, const QKeyEvent & event)
{
    if(event.key() == Qt::Key_Space)
    {
        return std::unique_ptr<RenderWidgetsBehavior>(new DefaultBehavior());
    }
    return std::nullopt;
}
