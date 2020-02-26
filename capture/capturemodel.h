#ifndef CAPTUREMODEL_H
#define CAPTUREMODEL_H
#include <QString>
#include <Qt>
#include <QPoint>
#include <QRect>
namespace Capture
{
    class Model
    {
    public:
        Model();
        int fps() const {return m_fps;}
        void fps(int val) {m_fps = val;}

        QRect recoredRect() const {return m_recoredRect;}
        void recoredRect(const QRect& rect){m_recoredRect= rect;}
    private:
        QRect m_recoredRect;
        int m_fps;
    };
}
#endif // CAPTUREMODEL_H
