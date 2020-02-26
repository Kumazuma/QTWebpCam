#include "editwindow.h"
#include "ui_editwindow.h"
#include <QTextStream>
#include "capturepreviewwindow.h"
EditWindow::EditWindow(FileImageStore* store, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow),
    m_store(store)
{
    ui->setupUi(this);
    m_store->setParent(this);
    for(size_t i = 0 ; i < m_store->size(); i++)
    {
        auto img = m_store->at(i);
        if(img)
        {
            QString item;
            QTextStream stream(&item);
            stream<<i<<" "
                <<", w:"<<img->width()
                <<", h: "
                <<img->height()
                <<", duration: "
               <<img->duration()
                <<"ms, file size: "<<img->sizeInBytes();
            stream.flush();
            QListWidgetItem* listItem = new QListWidgetItem(item,ui->listWidget);
            listItem->setData(Qt::UserRole, i);

            ui->listWidget->addItem(listItem);
        }

    }
    connect(ui->listWidget,&QListWidget::itemClicked,[this](QListWidgetItem *item){
        int index = qvariant_cast<int>(item->data(Qt::UserRole));
        auto frame = this->m_store->at(index);
        if(!frame)
            return;
        auto img = this->m_store->getImage(*frame);
        auto window = new CapturePreviewWindow(nullptr, img);
        window->show();
    });
}

EditWindow::~EditWindow()
{
    delete ui;
}
