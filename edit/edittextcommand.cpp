#include "edittextcommand.h"

EditTextCommand::EditTextCommand(
        EditPresenter &presenter,
        const std::shared_ptr<FileImageStore> &store,
        const QString& text,
        size_t start,
        std::optional<size_t> end):
    QUndoCommand("텍스트 삽입"),
    m_presenter(presenter),
    m_text(text),
    m_store(store),
    m_start(start),
    m_end(end.value_or(start))
{

}
EditTextCommand::~EditTextCommand()
{
}

void EditTextCommand::redo()
{
    m_presenter.Redo(this);
}

void EditTextCommand::undo()
{
    m_presenter.Undo(this);
}
std::shared_ptr<FileImageStore> EditTextCommand::swap(const std::shared_ptr<FileImageStore> &)
{
    std::shared_ptr<FileImageStore> t = m_store;
    m_store = store;
    m_store->unload();
    return t;
}
