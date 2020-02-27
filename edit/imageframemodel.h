#ifndef IMAGEFRAMEMODEL_H
#define IMAGEFRAMEMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include "../capture/fileimagestore.h"
#include <memory>
#include <map>
class ImageFrameModel : public QAbstractListModel
{
    Q_OBJECT
private:
    const FileImageStore& m_store;
    std::shared_ptr<std::map<int, QImage>> m_thumbnails;
public:
    ImageFrameModel(const FileImageStore& store, QObject* parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &parent, int role = Qt::DisplayRole)const  override;
};

#endif // IMAGEFRAMEMODEL_H
