QT       += core gui
QT += concurrent
LIBS += -lz
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
DEPENDPATH += $$PWD/libwebp-msw-x64
LIBS += -L$$PWD/libwebp-msw-x64/ -lwebpmux
LIBS += -L$$PWD/libwebp-msw-x64/ -lwebpdemux
LIBS += -L$$PWD/libwebp-msw-x64/ -lwebp
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
    edit/editcropcommand.cpp \
    edit/editdeletecommand.cpp \
    edit/editrenderwidget.cpp \
    edit/edittextcommand.cpp \
    fileimagestore.cpp \
    foreachthread.cpp \
    imageframe.cpp \
    capture/capturemodel.cpp \
    capturepreviewwindow.cpp \
    edit/editmodel.cpp \
    edit/editpresenter.cpp \
    edit/editwindow.cpp \
    edit/encodingprogressdialog.cpp \
    edit/imageframemodel.cpp \
    encodingthread.cpp \
    main.cpp \
    webpencoder.cpp\
    ./capture/mainwindow.cpp \
    ./capture/presenter.cpp \
    ./capture/capturer.cpp
HEADERS += \
    edit/editcropcommand.h \
    edit/editdeletecommand.h \
    edit/editrenderwidget.h \
    edit/edittextcommand.h \
    fileimagestore.h \
    foreachthread.h \
    imageframe.h \
    capture/capturemodel.h \
    ./capture/capturer.h \
    capturepreviewwindow.h \
    edit/editmodel.h \
    edit/editpresenter.h \
    edit/editwindow.h \
    edit/encodingprogressdialog.h \
    edit/imageframemodel.h \
    encodingthread.h \
    mainwindow.h \
    webpencoder.h\
    ./capture/presenter.h\
    webp/decode.h\
    webp/encode.h\
    webp/demux.h\
    webp/mux.h\
    webp/mux_types.h\
    webp/types.h
FORMS += \
    capturepreviewwindow.ui \
    edit/editwindow.ui \
    edit/encodingprogressdialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/icons.qrc
