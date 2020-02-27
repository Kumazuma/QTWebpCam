#include "imageframemodel.h"
#include <QIcon>
ImageFrameModel::ImageFrameModel(const FileImageStore& store, QObject* parent):
    QAbstractListModel(parent),
    m_store(store),
    m_thumbnails(new std::map<int, QImage>())
{

}

int ImageFrameModel::rowCount(const QModelIndex &) const
{
    return m_store.size();
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
    auto frame = m_store.at(parent.row());
    if(role == Qt::DecorationRole)
    {
        auto f = m_thumbnails->find(parent.row());
        if(f == m_thumbnails->end())
        {
            auto temp = m_store.getImage(*frame);
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
