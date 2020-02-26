#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "capture/presenter.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    Presenter* m_presenter;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent* event) override;
    void moveEvent(QMoveEvent* event) override;
    void showEvent(QShowEvent * event) override;
    bool nativeEvent(const QByteArray& eventType, void* msg, long * result) override;

private slots:
    void onClickedClose();
    void onClickStart();
    void onSpinWidthChange(int val);
    void onSpinHeightChange(int val);
    void onSpinPosXChanged(int val);
    void onSpinPosYChanged(int val);
    void onChangedRecoredRect(const QRect& rect);
    void onFpsChanged(const QString& text);
    void showEditWindow(FileImageStore* store);
};
#endif // MAINWINDOW_H
