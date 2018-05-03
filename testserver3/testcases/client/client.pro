#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T22:17:01
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp client.cpp serverinfosearcher.cpp \
    player.cpp \
    playerwidget.cpp ../../tool.cpp ../../videosource.cpp ../../pvd.cpp ../../cppjson/json_reader.cpp \
    ../../cppjson/json_value.cpp \
    ../../cppjson/json_writer.cpp

HEADERS  += mainwindow.h client.h serverinfosearcher.h \
    player.h \
    playerwidget.h ../../tool.h   ../../videosource.h  ../../pvd.h

FORMS    += mainwindow.ui
#include(../common/common.pri)

INCLUDEPATH+=../../  ../../cppjson/include
CONFIG+=c++11
DEFINES+=IS_UNIX
    LIBS+= -lopencv_core -lopencv_highgui \
       -lopencv_objdetect -lopencv_imgproc -lopencv_ml -lopencv_highgui  -lopencv_video  -lX11

