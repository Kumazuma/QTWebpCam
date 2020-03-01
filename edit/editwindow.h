#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "editpresenter.h"
#include "editrenderwidget.h"
namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::EditWindow *ui;
    EditPresenter* m_presenter = nullptr;
    EditRenderWidget * ui_renderWidget = nullptr;
public:
    explicit EditWindow(FileImageStore* store, QWidget *parent = nullptr);
    ~EditWindow();
protected:

protected slots:
    void selectFrame(const ImageFrame&  frame);
    void playState(bool state);
    void save(bool);

};

#endif // EDITWINDOW_H
