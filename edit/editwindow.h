#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "../capture/fileimagestore.h"
namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::EditWindow *ui;
    FileImageStore * m_store;
protected slots:
    void selectFrame(const ImageFrame&  frame);
public:
    explicit EditWindow(FileImageStore* store, QWidget *parent = nullptr);
    ~EditWindow();


};

#endif // EDITWINDOW_H
