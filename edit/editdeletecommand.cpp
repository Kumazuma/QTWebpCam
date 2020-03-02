#include "editdeletecommand.h"

EditDeleteCommand::EditDeleteCommand(EditPresenter &presenter,
                                     const std::shared_ptr<FileImageStore>& store,
                                     size_t start,
                                     std::optional<size_t> end):
    QUndoCommand("프레임 삭제"),
    m_presenter(presenter),
    m_store(store),
    m_start(start),
    m_end(end.value_or(start))
{
    setText("프레임삭제");
}

EditDeleteCommand::~EditDeleteCommand()
{
}

void EditDeleteCommand::redo()
{
    m_presenter.Redo(this);
}

void EditDeleteCommand::undo()
{
    m_presenter.Undo(this);
}
std::shared_ptr<FileImageStore> EditDeleteCommand::swap(const std::shared_ptr<FileImageStore>& store)
{
    std::shared_ptr<FileImageStore> t = m_store;
    m_store = store;
    m_store->unload();
    return t;
}
