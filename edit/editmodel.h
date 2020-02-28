#ifndef EDITMODEL_H
#define EDITMODEL_H

#include "../capture/fileimagestore.h"
class EditModel
{
private:
    FileImageStore * m_store;
    bool m_isPlay;
    std::optional<size_t> m_selectedIndex;

public:
    EditModel(FileImageStore* store);
    FileImageStore*& store(){return m_store;}
    std::optional<size_t> selectedIndex(){return m_selectedIndex;}
    void setSelectedIndex(const std::optional<size_t>& val){m_selectedIndex = val;}
    bool isPlay(){return m_isPlay;}
    void setPlay(bool val){m_isPlay = val;}
};

#endif // EDITMODEL_H
