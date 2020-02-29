#ifndef IMAGEFRAMEMODEL_H
#define IMAGEFRAMEMODEL_H
#include <QObject>
#include <QAbstractListModel>
#include "editpresenter.h"
#include <memory>
#include <map>
class ImageFrameModel : public QAbstractListModel
{
    Q_OBJECT
private:
    const EditPresenter& m_presenter;
    std::shared_ptr<std::map<int, QImage>> m_thumbnails;
public:
    ImageFrameModel(const EditPresenter& presenter, QObject* parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &parent, int role = Qt::DisplayRole)const  override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
protected slots:
    void updateImageStore();
};

#endif // IMAGEFRAMEMODEL_H
