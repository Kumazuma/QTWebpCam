#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "editpresenter.h"
namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::EditWindow *ui;
    EditPresenter* m_presenter = nullptr;

protected slots:
    void selectFrame(const ImageFrame&  frame);
    void playState(bool state);
public:
    explicit EditWindow(FileImageStore* store, QWidget *parent = nullptr);
    ~EditWindow();


};

#endif // EDITWINDOW_H
