#ifndef EDITRENDERWIDGET_H
#define EDITRENDERWIDGET_H
#include <QGraphicsView>
#include "editpresenter.h"
#include <optional>
#include <QGraphicsPathItem>
#include "renderwidgetsbehavior.h"
#include <QScopedPointer>
class EditRenderWidget :public QGraphicsView
{
    Q_OBJECT
    friend struct RenderWidgetsBehavior;
private:
    EditPresenter& m_presenter;
    QRect m_cropRect;
    QGraphicsPathItem* m_pathItem;
    qreal m_scale;
    std::unique_ptr<RenderWidgetsBehavior> m_behavior;
public:
    EditRenderWidget(EditPresenter& presenter, QWidget* parent = nullptr);
    ~EditRenderWidget();

    qreal scale(){return m_scale;}
    void setScale(qreal val);
    auto imageSize(){return m_presenter.imageStore().imageSize();}
    void setCropRect(const QRect& rect);
    auto cropRect(){return m_cropRect;}
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
protected slots:
    void selectedFrame(const ImageFrame& frame);
    void reloadImageStore();
signals:
    void cropRectUpdate(QRect);
};

#endif // EDITRENDERWIDGET_H
