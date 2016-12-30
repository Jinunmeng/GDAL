#-------------------------------------------------
#
# Project created by QtCreator 2016-12-23T15:38:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageProcess
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cprogressdlg.cpp \
    cpolygonizedlg.cpp \
    imagepyramidsdlg.cpp \
    sievefilterdlg.cpp \
    createcontourdlg.cpp

HEADERS  += mainwindow.h \
    progressbase.h \
    cprogressdlg.h \
    cpolygonizedlg.h \
    imagepyramidsdlg.h \
    sievefilterdlg.h \
    createcontourdlg.h

FORMS    += mainwindow.ui \
    cpolygonizedlg.ui \
    imagepyramidsdlg.ui \
    sievefilterdlg.ui \
    createcontourdlg.ui
win32: LIBS += -LF:/GDAL-Qt/lib/ -lgdal_i

INCLUDEPATH += F:/GDAL-Qt/include
DEPENDPATH += F:/GDAL-Qt/include
