#ifndef RENDERWIDGETSBEHAVIOR_H
#define RENDERWIDGETSBEHAVIOR_H
#include<QMouseEvent>
#include<QKeyEvent>
#include<QGraphicsItem>
#include<memory>
#include<optional>
class EditRenderWidget;
struct RenderWidgetsBehavior;
using ReturnType = std::optional<std::unique_ptr<RenderWidgetsBehavior>>;
struct RenderWidgetsBehavior
{
    virtual ~RenderWidgetsBehavior(){};
    virtual ReturnType wheelEvent(EditRenderWidget&, const QWheelEvent&){return std::nullopt;};
    virtual ReturnType mouseMoveEvent(EditRenderWidget&, const QMouseEvent&){return std::nullopt;};
    virtual ReturnType mousePressEvent(EditRenderWidget&,const QMouseEvent&){return std::nullopt;};
    virtual ReturnType mouseReleaseEvent(EditRenderWidget&, const QMouseEvent&){return std::nullopt;};
    virtual ReturnType keyPressEvent(EditRenderWidget&, const QKeyEvent&){return std::nullopt;};
    virtual ReturnType keyReleaseEvent(EditRenderWidget&, const QKeyEvent&){return std::nullopt;};
};
class DefaultBehavior : public RenderWidgetsBehavior
{
private:
    std::optional<std::array<qint8,2>> m_sizingAxis;
    QPointF m_prevPoint;
private:
    std::array<qint8,2> calcSizingDirections(const QRect& cropRect, const QPointF& point);
    QCursor getSizingDirectionCursor(const std::array<qint8, 2>& axis);
public:
    DefaultBehavior();
    ~DefaultBehavior();
    ReturnType wheelEvent(EditRenderWidget&, const QWheelEvent&) override;
    ReturnType mouseMoveEvent(EditRenderWidget&, const QMouseEvent&) override;
    ReturnType mousePressEvent(EditRenderWidget&, const QMouseEvent&) override;
    ReturnType mouseReleaseEvent(EditRenderWidget&, const QMouseEvent&) override;
    ReturnType keyPressEvent(EditRenderWidget&, const QKeyEvent&) override;
};
class ScaleBehavior : public RenderWidgetsBehavior
{
public:
    ReturnType wheelEvent(EditRenderWidget&, const QWheelEvent&)override;
    ReturnType keyReleaseEvent(EditRenderWidget&, const QKeyEvent&) override;
};
class HScrollBehavior: public RenderWidgetsBehavior
{
public:
    ReturnType wheelEvent(EditRenderWidget&, const QWheelEvent&)override;
    ReturnType keyReleaseEvent(EditRenderWidget&, const QKeyEvent&) override;
};
class GrabMoveBehavior : public RenderWidgetsBehavior
{
private:
    QPoint m_prev;
public:
    GrabMoveBehavior(EditRenderWidget&);
    ReturnType mouseMoveEvent(EditRenderWidget&, const QMouseEvent&)override;
    ReturnType keyReleaseEvent(EditRenderWidget&, const QKeyEvent&) override;
};
#endif // RENDERWIDGETSBEHAVIOR_H
