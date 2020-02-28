#include "editwindow.h"
#include "ui_editwindow.h"
#include <QTextStream>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "capturepreviewwindow.h"
#include "imageframemodel.h"
#include <QFileDialog>

EditWindow::EditWindow(FileImageStore* store, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow),
    m_presenter(new EditPresenter(store, this))
{
    //ui->retake->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->setupUi(this);

    auto model = new ImageFrameModel(m_presenter->imageStore(),ui->listView);

    ui->listView->setModel(model);
    auto selectionModel = ui->listView->selectionModel();
    connect(m_presenter, &EditPresenter::currentImageFrame, this, &EditWindow::selectFrame);
    connect(m_presenter, &EditPresenter::changePlayState, this, &EditWindow::playState);

    connect(ui->actionSave, &QAction::triggered, this, &EditWindow::save);
    connect(ui->actionPlay, &QAction::changed, [this]()
    {
        if(ui->actionPlay->isChecked())
            m_presenter->play();
        else
            m_presenter->stop();
    });
    connect(selectionModel,
            &QItemSelectionModel::currentChanged,
            [this](const QModelIndex &index, const QModelIndex &)
    {
        if(!m_presenter->isPlaying())
        {
            ImageFrame frame = index.data(Qt::UserRole).value<ImageFrame>();
            auto row = *m_presenter->currentFrameIndex();
            if(row != index.row())
                m_presenter->setCurrentImageFrame(frame);
        }
    });
}

EditWindow::~EditWindow()
{
    delete ui;
}
void EditWindow::selectFrame(const ImageFrame& frame)
{
    auto index = ui->listView->model()->index(*m_presenter->currentFrameIndex() ,0);
    auto selectionModel = ui->listView->selectionModel();
    if(m_presenter->isPlaying())
    {
        ui->listView->selectionModel()->clear();
    }
    if(!selectionModel->isSelected(index))
    {
        selectionModel->setCurrentIndex(index,QItemSelectionModel::Select);
    }

    auto image = m_presenter->getImageFromImageFrame(frame);
    auto scene = new QGraphicsScene(this->ui->graphicsView);
    auto pixmap = QPixmap::fromImage(image);
    scene->addPixmap(pixmap);
    this->ui->graphicsView->setScene(scene);
}

void EditWindow::playState(bool state)
{
    ui->actionPlay->setChecked(state);
}

void EditWindow::currentSelect(const QModelIndex &index, const QModelIndex &)
{

}

void EditWindow::save(bool )
{
    QString filePath =QFileDialog::getSaveFileName(this,
            tr("Save Anim Webp File"), "",
            tr("Webp file (*.webp);;"));
    if(filePath.isEmpty())
        return;
    m_presenter->saveAnimWebp(filePath);
}

