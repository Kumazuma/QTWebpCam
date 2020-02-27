#ifndef EDITPRESENTER_H
#define EDITPRESENTER_H

#include <QObject>

class EditPresenter : public QObject
{
    Q_OBJECT
public:
    explicit EditPresenter(QObject *parent = nullptr);

signals:

};

#endif // EDITPRESENTER_H
