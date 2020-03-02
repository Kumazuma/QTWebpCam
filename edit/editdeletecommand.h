#ifndef EDITDELETECOMMAND_H
#define EDITDELETECOMMAND_H
#include <QUndoCommand>
#include "editpresenter.h"
#include "../fileimagestore.h"
class EditDeleteCommand:public QUndoCommand
{
private:
    EditPresenter& m_presenter;
    std::shared_ptr<FileImageStore> m_store;
    size_t m_start;
    size_t m_end;
public:
    EditDeleteCommand(EditPresenter &presenter,
                      const std::shared_ptr<FileImageStore>& store,
                      size_t start,
                      std::optional<size_t> end = std::nullopt);
    virtual ~EditDeleteCommand() override;
    void redo() override;
    void undo() override;
    size_t start(){return m_start;}
    size_t end(){return m_end;}
    std::shared_ptr<FileImageStore> swap(const std::shared_ptr<FileImageStore>&);
};

#endif // EDITDELETECOMMAND_H
