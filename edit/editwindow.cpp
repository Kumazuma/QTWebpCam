#include "editwindow.h"
#include "ui_editwindow.h"
#include <QTextStream>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include "../mainwindow.h"
#include "capturepreviewwindow.h"
#include "imageframemodel.h"

#include "encodingprogressdialog.h"
EditWindow::EditWindow(FileImageStore* store, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow),
    m_presenter(new EditPresenter(store, this))
{
    //ui->retake->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
    ui->setupUi(this);
    ui_renderWidget = new EditRenderWidget(*m_presenter, this);
    ui->verticalLayout_2->addWidget(ui_renderWidget);
    ui_renderWidget->show();
    auto model = new ImageFrameModel(*m_presenter,ui->listView);
    ui->listView->setModel(model);
    ui->undoView->setStack(m_presenter->undoStack());

    updateImageStore();
    auto selectionModel = ui->listView->selectionModel();
    connect(m_presenter, &EditPresenter::currentImageFrame,
            this, &EditWindow::selectFrame);
    connect(m_presenter, &EditPresenter::updateImageStore,
            this, &EditWindow::updateImageStore);
    connect(m_presenter, &EditPresenter::changePlayState,
            this, &EditWindow::playState);
    connect(ui_renderWidget, &EditRenderWidget::cropRectUpdate,
            m_presenter, &EditPresenter::setCropRect);
    connect(ui->actionCrop, &QAction::triggered, m_presenter, &EditPresenter::crop);
    connect(ui->actionDelete, &QAction::triggered, [this](bool)
    {
        auto s = ui->listView->selectionModel()->selectedIndexes();
        if(s.size() == 0)
            return;
        size_t start = s.first().row();
        size_t end = s.last().row() + 1;
        m_presenter->deleteFrame(start, end);

    });
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
            qDebug()<<__PRETTY_FUNCTION__;
            ImageFrame frame = index.data(Qt::UserRole).value<ImageFrame>();
            auto row = *m_presenter->currentFrameIndex();
            if(row != index.row())
                m_presenter->setCurrentImageFrame(frame);
        }
    });
    connect(ui->actionNew, &QAction::triggered, [this](bool)
    {
        auto w = new MainWindow();
        w->show();
        this->close();
        this->deleteLater();
    });
}

EditWindow::~EditWindow()
{
    qDebug()<<__PRETTY_FUNCTION__;
    delete ui;
    delete m_presenter;
}

void EditWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<__PRETTY_FUNCTION__;
    QMainWindow::closeEvent(event);
    this->deleteLater();
}
void EditWindow::selectFrame(const ImageFrame& )
{
    qDebug()<<__PRETTY_FUNCTION__;
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
}

void EditWindow::playState(bool state)
{
    ui->actionPlay->setChecked(state);
}

void EditWindow::save(bool )
{
    QString filePath =QFileDialog::getSaveFileName(this,
            tr("Save Anim Webp File"), "",
            tr("Webp file (*.webp);;"));
    if(!filePath.isEmpty())
    {
        m_presenter->setFilePath(filePath);
        auto dialog = new EncodingProgressDialog(*m_presenter, this);
        // dialog->show();
         dialog->exec();
         delete dialog;
        return;
    }
}

void EditWindow::updateImageStore()
{
    ui->treeWidget->clear();
    auto * rootItem = new QTreeWidgetItem();
    QTreeWidgetItem * childItem = nullptr;
    rootItem->setText(0, "information");
    childItem = new QTreeWidgetItem();
    childItem->setText(0, "width");
    childItem->setText(1, QString("%1").arg( m_presenter->imageStore().imageSize().width()));
    rootItem->addChild(childItem);
    childItem = new QTreeWidgetItem();
    childItem->setText(0, "height");
    childItem->setText(1, QString("%1").arg( m_presenter->imageStore().imageSize().height()));
    rootItem->addChild(childItem);
    childItem = new QTreeWidgetItem();
    childItem->setText(0, "frame");
    childItem->setText(1, QString("%1").arg( m_presenter->imageStore().size()));
    rootItem->addChild(childItem);
    ui->treeWidget->addTopLevelItem(rootItem);
    ui->treeWidget->setHeaderLabels({"",""});
    ui->treeWidget->setHeaderHidden(true);
    ui->treeWidget->expandAll();
}

