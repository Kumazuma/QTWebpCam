#include "mainwindow.h"

#include <QApplication>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile cssFile("./design.css");
    if(!cssFile.open(QFile::ReadOnly))
    {
        return -1;
    }
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":resource");
    a.setStyleSheet(cssFile.readAll());
    MainWindow w;
    w.show();
    return a.exec();
}
