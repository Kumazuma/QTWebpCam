#ifndef EDITCROPCOMMAND_H
#define EDITCROPCOMMAND_H
#include <QUndoCommand>
#include "editpresenter.h"
#include "../fileimagestore.h"

class EditCropCommand:public QUndoCommand
{
private:
    EditPresenter& m_presenter;
    std::shared_ptr<FileImageStore> m_store;
    QRect m_cropRect;
public:
    EditCropCommand(EditPresenter& presenter,
                      const std::shared_ptr<FileImageStore>& store,
                      QRect cropRect);
    virtual ~EditCropCommand() override;
    void redo() override;
    void undo() override;
    QRect cropRect(){return m_cropRect;}
    std::shared_ptr<FileImageStore> swap(std::shared_ptr<FileImageStore>&);

};

#endif // EDITCROPCOMMAND_H
