#include "imageframemodel.h"
#include <QIcon>
ImageFrameModel::ImageFrameModel(const EditPresenter& presenter, QObject* parent):
    QAbstractListModel(parent),
    m_presenter(presenter),
    m_thumbnails(new std::map<int, QImage>())
{
    connect(&m_presenter, &EditPresenter::updateImageStore,
            this, &ImageFrameModel::updateImageStore);
}

int ImageFrameModel::rowCount(const QModelIndex &) const
{
    return m_presenter.imageStore().size();
}

int ImageFrameModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant ImageFrameModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return "duration";

        }
    }
    return QVariant();
}



QVariant ImageFrameModel::data(const QModelIndex &parent, int role) const
{
    const auto& store = m_presenter.imageStore();
    auto frame = store.at(parent.row());
    if(role == Qt::DecorationRole)
    {
        auto f = m_thumbnails->find(parent.row());
        if(f == m_thumbnails->end())
        {
            auto temp = store.getImage(*frame);
            auto temp2 = temp.scaled(64,64);
            temp.detach();
            auto it = m_thumbnails->insert(std::pair<int, QImage>(parent.row(), temp2));
            return it.second;
        }
        return f->second;
    }
    if(role == Qt::DisplayRole)
    {
        return QString("%1ms")
                .arg(frame->duration());
    }
    else if(role == Qt::UserRole)
    {
        return qVariantFromValue(*frame);
    }
    return QVariant();
}

Qt::ItemFlags ImageFrameModel::flags(const QModelIndex &) const
{
    return
            Qt::ItemFlag::ItemIsSelectable |
            Qt::ItemFlag::ItemIsEnabled |
            Qt::ItemNeverHasChildren;
}

void ImageFrameModel::updateImageStore()
{
    m_thumbnails->clear();
    emit dataChanged(index(0) , index(m_presenter.imageStore().size()));
}
