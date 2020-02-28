#ifndef ENCODINGPROGRESSDIALOG_H
#define ENCODINGPROGRESSDIALOG_H

#include <QDialog>
#include "editpresenter.h"
#include <QAbstractButton>
namespace Ui {
class EncodingProgressDialog;
}

class EncodingProgressDialog : public QDialog
{
    Q_OBJECT
private:
    EditPresenter & m_presenter;
    Ui::EncodingProgressDialog *ui;
public:
    explicit EncodingProgressDialog(EditPresenter & presenter, QWidget *parent = nullptr);
    ~EncodingProgressDialog();

protected slots:
    void progressEncode(int current, int max);
    void completeEncode(const QString&);
    void buttonClicked(QAbstractButton* btn);
};

#endif // ENCODINGPROGRESSDIALOG_H
