# -------------------------------------------------
# Project created by QtCreator 2010-05-21T13:41:55
# -------------------------------------------------
QT += opengl
TARGET = solid2d
TEMPLATE = app
QMAKE_INCDIR += ../../include
QMAKE_LIBDIR += ../../lib
QMAKE_LIBS += -ldfm_core
SOURCES += main.cpp \
    mainwindow.cpp \
    glwidget.cpp \
    dialog_solid2d.cpp
HEADERS += mainwindow.h \
    glwidget.h \
    dialog_solid2d.h
FORMS += mainwindow.ui \
    dialog_solid2d.ui