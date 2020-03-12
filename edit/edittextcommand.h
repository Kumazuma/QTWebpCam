#ifndef EDITTEXTCOMMAND_H
#define EDITTEXTCOMMAND_H
#include <QUndoCommand>
#include "editpresenter.h"
#include "../fileimagestore.h"

class EditTextCommand:public QUndoCommand
{
private:
    EditPresenter& m_presenter;
    std::shared_ptr<FileImageStore> m_store;
    size_t m_start;
    size_t m_end;
    QString m_text;
public:
    EditTextCommand(EditPresenter &presenter,
                    const std::shared_ptr<FileImageStore>& store,
                    const QString& text,
                    size_t start,
                    std::optional<size_t> end = std::nullopt);
    virtual ~EditTextCommand() override;
    void redo() override;
    void undo() override;
    size_t start(){return m_start;}
    size_t end(){return m_end;}
    std::shared_ptr<FileImageStore> swap(const std::shared_ptr<FileImageStore>&);
};

#endif // EDITTEXTCOMMAND_H
