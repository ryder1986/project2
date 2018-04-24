QT+=network
QT       -= gui

TARGET = server
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    videoprocessor.cpp \
    server.cpp \
    processeddatasender.cpp \
    clientsession.cpp \
    cameramanager.cpp \
    camera.cpp C4Common.cpp pvdprocessor.cpp tool.cpp videosource.cpp filedatabase.cpp
install_files.files+=config.json  hogcascade_pedestrians.xml combined.txt.model combined2.txt.model
install_files.path=$$OUT_PWD/
INSTALLS +=install_files

#include(../common/common.pri)

HEADERS += \
    videoprocessor.h \
    server.h \
    processeddatasender.h \
    clientsession.h \
    cameramanager.h \
    camera.h C4Common.h  pvdprocessor.h tool.h pvd.h videosource.h filedatabase.h
#unix|win32: LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor
#INCLUDEPATH += $$PWD/../PvdProcessor
#DEPENDPATH += $$PWD/../PvdProcessor
#win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/PvdProcessor.lib
#else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/libPvdProcessor.a
# LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PvdProcessor/release/ -lPvdProcessor
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PvdProcessor/debug/ -lPvdProcessor
#else:unix: LIBS += -L$$OUT_PWD/../PvdProcessor/ -lPvdProcessor

#INCLUDEPATH += $$PWD/../PvdProcessor
#DEPENDPATH += $$PWD/../PvdProcessor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/release/libPvdProcessor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/debug/libPvdProcessor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/release/PvdProcessor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/debug/PvdProcessor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PvdProcessor/libPvdProcessor.a
DEFINES +=IS_UNIX
CONFIG +=c++11

INCLUDEPATH+=./include
QMAKE_CXXFLAGS+=-w
unix{
DEFINES +=IS_UNIX
QMAKE_CXXFLAGS+=-w
}
unix{
CVPATH="/root/repo-github/libs/third-party/opencv"
CV_PREFIX="/opencv-linux-x86"
ALGPATH="/root/repo-github/libs/alg"
}
unix{
    DEFINES += IS_UNIX
    QMAKE_CXXFLAGS+="-std=c++11"
    INCLUDEPATH +="$$CVPATH/$$CV_PREFIX/include"
    INCLUDEPATH +="$$ALGPATH/include"
    message(alg  in $$ALGPATH/include)
    LIBS+=-L$$CVPATH/$$CV_PREFIX/lib -lopencv_core -lopencv_highgui \
       -lopencv_objdetect -lopencv_imgproc -lopencv_ml   -lopencv_video  -lX11

    QMAKE_LIBS+=-lpthread
}
   INCLUDEPATH +=../server ../common


win32{
OPENCV_PATH=E:\repo-github\libs\third-party\opencv\opencv-vs2013-x86
    INCLUDEPATH +="$$OPENCV_PATH\include"
# LIBS+=-L$$OPENCV_PATH\lib   -lopencv_core249 -lopencv_highgui249 \
#        -lopencv_objdetect249 -lopencv_imgproc249 -lopencv_ml249  -lopencv_video249
 LIBS+=-L$$OPENCV_PATH\lib   -lopencv_core249d -lopencv_highgui249d \
        -lopencv_objdetect249d -lopencv_imgproc249d -lopencv_ml249d  -lopencv_video249d
INCLUDEPATH+="C:\Program Files\Microsoft SDKs\Windows\v7.1A\Include"
}

INCLUDEPATH+=cppjson/include
SOURCES += \
    cppjson/json_reader.cpp \
    cppjson/json_value.cpp \
    cppjson/json_writer.cpp
