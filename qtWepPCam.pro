QT       += core gui
LIBS += -lz
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    capture/capturemodel.cpp \
    capture/fileimagestore.cpp \
    capture/imageframe.cpp \
    capturepreviewwindow.cpp \
    edit/editwindow.cpp \
    main.cpp \
    ./capture/mainwindow.cpp \
    ./capture/presenter.cpp \
    ./capture/capturer.cpp
HEADERS += \
    capture/capturemodel.h \
    ./capture/capturer.h \
    capture/fileimagestore.h \
    capture/imageframe.h \
    capturepreviewwindow.h \
    edit/editwindow.h \
    mainwindow.h \
    ./capture/presenter.h

FORMS += \
    capturepreviewwindow.ui \
    edit/editwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
