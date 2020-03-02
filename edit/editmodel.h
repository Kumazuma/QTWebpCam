#ifndef EDITMODEL_H
#define EDITMODEL_H
#include <QUndoStack>
#include <memory>
#include "fileimagestore.h"
class EditModel
{
private:
    std::shared_ptr<FileImageStore> m_store;
    bool m_isPlay;
    std::optional<size_t> m_selectedIndex;
    QString m_filePath;
    QRect m_cropRect;
    QUndoStack * m_undoStack;
public:
    EditModel(FileImageStore* store);
    std::shared_ptr<FileImageStore> store()const {return m_store;}
    std::shared_ptr<FileImageStore>& store() {return m_store;}
    std::optional<size_t> selectedIndex(){return m_selectedIndex;}
    void setSelectedIndex(const std::optional<size_t>& val){m_selectedIndex = val;}
    bool isPlay(){return m_isPlay;}
    void setPlay(bool val){m_isPlay = val;}
    QString filePath(){return m_filePath;}
    void setFilePath(const QString& val){m_filePath = val;}
    QRect cropRect(){return m_cropRect;}
    void setCropRect(const QRect& val){m_cropRect= val;}
    QUndoStack* undoStack(){return m_undoStack;}
    void setUndoStack(QUndoStack * val){m_undoStack = val;}
};

#endif // EDITMODEL_H
