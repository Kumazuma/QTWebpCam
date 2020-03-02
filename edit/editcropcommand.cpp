#include "editcropcommand.h"


EditCropCommand::EditCropCommand(EditPresenter &presenter, const std::shared_ptr<FileImageStore>& store, QRect cropRect):
    QUndoCommand("영역 자르기"),
    m_presenter(presenter),
    m_store(store),
    m_cropRect(cropRect)
{
    setText("영역 자르기");
}

EditCropCommand::~EditCropCommand()
{
}
void EditCropCommand::redo()
{
    m_presenter.Redo(this);
}

void EditCropCommand::undo()
{
    m_presenter.Undo(this);
}
std::shared_ptr<FileImageStore> EditCropCommand::swap(std::shared_ptr<FileImageStore>& store)
{
    std::shared_ptr<FileImageStore> t = m_store;
    m_store = store;
    m_store->unload();
    return t;
}
