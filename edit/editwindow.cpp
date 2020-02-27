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
    m_store(store)
{
    //ui->retake->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->setupUi(this);
    m_store->setParent(this);
    auto model = new ImageFrameModel(*m_store,ui->listView);
    ui->listView->setModel(model);
    connect(ui->listView, &QTreeView::clicked,[this](const QModelIndex &index){
        ImageFrame frame = index.data(Qt::UserRole).value<ImageFrame>();
        selectFrame(frame);
    });

}

EditWindow::~EditWindow()
{
    delete ui;
}
void EditWindow::selectFrame(const ImageFrame& frame)
{
    auto img = this->m_store->getImage(frame);
    auto scene = new QGraphicsScene(this->ui->graphicsView);
    auto pixmap = QPixmap::fromImage(img);
    scene->addPixmap(pixmap);
    this->ui->graphicsView->setScene(scene);
}

