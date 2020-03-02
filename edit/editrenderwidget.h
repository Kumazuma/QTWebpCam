#ifndef EDITRENDERWIDGET_H
#define EDITRENDERWIDGET_H
#include <QGraphicsView>
#include "editpresenter.h"
#include <optional>
#include <QGraphicsPathItem>
class EditRenderWidget :public QGraphicsView
{
    Q_OBJECT
private:
    EditPresenter& m_presenter;
    QRect m_cropRect;
    std::optional<std::array<qint8,2>> m_sizingAxis;
    QPointF m_prevPoint;
    QGraphicsPathItem* m_pathItem;
public:
    EditRenderWidget(EditPresenter& presenter, QWidget* parent = nullptr);
    ~EditRenderWidget();
private:
    std::array<qint8,2> calcSizingDirections(const QPointF& point);
    QCursor getSizingDirectionCursor(const std::array<qint8, 2>& axis);
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
protected slots:
    void selectedFrame(const ImageFrame& frame);
    void reloadImageStore();
signals:
    void cropRect(QRect);
};

#endif // EDITRENDERWIDGET_H
