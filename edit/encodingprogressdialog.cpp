#include "encodingprogressdialog.h"
#include "ui_encodingprogressdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
EncodingProgressDialog::EncodingProgressDialog(EditPresenter & presenter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EncodingProgressDialog),
    m_presenter(presenter)
{
    ui->setupUi(this);
    QString filePath =QFileDialog::getSaveFileName(this,
            tr("Save Anim Webp File"), "",
            tr("Webp file (*.webp);;"));
    if(filePath.isEmpty())
        close();
    connect(&m_presenter, &EditPresenter::progressEncode,
            this,&EncodingProgressDialog::progressEncode);
    connect(&m_presenter, &EditPresenter::completeFileSave,
            this, &EncodingProgressDialog::completeEncode);
    connect(ui->buttonBox, &QDialogButtonBox::clicked,
            this, &EncodingProgressDialog::buttonClicked);
    m_presenter.saveAnimWebp(filePath);
}

EncodingProgressDialog::~EncodingProgressDialog()
{
    delete ui;
}

void EncodingProgressDialog::progressEncode(int current, int max)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(current);
}

void EncodingProgressDialog::completeEncode(const QString &)
{
    QMessageBox::information(this, "information", "완료되었습니다");
    close();
}

void EncodingProgressDialog::buttonClicked(QAbstractButton *btn)
{
    auto cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);
    if(cancelButton == btn)
    {
        m_presenter.cancelEncode();
        close();
    }
}
