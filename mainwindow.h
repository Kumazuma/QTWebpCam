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
    bool m_isResizable = true;
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
    void onSpinWidthChange(int val);
    void onSpinHeightChange(int val);
    void onSpinPosXChange(int val);
    void onSpinPosYChange(int val);
    void onChangedRecoredRect(const QRect& rect);
    void onFpsChanged(const QString& text);
    void showEditWindow(FileImageStore* store);
    void startRecord();
};
#endif // MAINWINDOW_H
