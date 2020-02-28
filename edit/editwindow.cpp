#include "editwindow.h"
#include "ui_editwindow.h"
#include <QTextStream>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "capturepreviewwindow.h"
#include "imageframemodel.h"


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
        ImageFrame frame = index.data(Qt::UserRole).value<ImageFrame>();
        m_presenter->setCurrentImageFrame(frame);
    });
}

EditWindow::~EditWindow()
{
    delete ui;
}
void EditWindow::selectFrame(const ImageFrame& frame)
{
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

