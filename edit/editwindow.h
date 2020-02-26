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
    FileImageStore * m_store;
public:
    explicit EditWindow(FileImageStore* store, QWidget *parent = nullptr);
    ~EditWindow();

private:
    Ui::EditWindow *ui;
};

#endif // EDITWINDOW_H
