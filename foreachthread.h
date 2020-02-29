#ifndef FOREACHTHREAD_H
#define FOREACHTHREAD_H
#include"fileimagestore.h"

#include<QThread>
class FrameDeleteThread : public QThread
{
public:
    FrameDeleteThread(FileImageStore& source, MemoryMapStoreBuilder& dest, QObject* parent);
    void setStart(size_t start);
    void setEnd(size_t end);
protected:
    void run() override;
private:
    FileImageStore& m_source;
    MemoryMapStoreBuilder& m_dest;
    size_t m_start;
    size_t m_end;
};

#endif // FOREACHTHREAD_H
