QT += core
QT -= gui

TARGET = testalg
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

#INCLUDEPATH+=../PvdProcessor

INCLUDEPATH+= include
INCLUDEPATH+=../common

CONFIG +=c++11

DEFINES+=IS_UNIX
QMAKE_CXXFLAGS +=-w


SOURCES +=  pvdprocessor1.cpp \
     teststaticlib.cpp  C4Common1.cpp fun.cpp

HEADERS +=  pvdprocessor1.h \
     teststaticlib.h   C4Common1.h

INCLUDEPATH+=cvlib/inc

#SOURCES+=cvlib/cxcopy.cpp
#SOURCES+=cvlib/cxarray.cpp
#SOURCES+=cvlib/loadsave.cpp
#SOURCES+=cvlib/cxalloc.cpp cvlib/cxerror.cpp


#SOURCES+=\
#cvlib/cvapprox.cpp    cvlib/cvLBP.cpp         cvlib/cvsumpixels.cpp  \
#cvlib/cxalloc.cpp   cvlib/cxcopy.cpp         cvlib/cxouttext.cpp    \
#cvlib/loadsave.cpp cvlib/cvcontours.cpp  cvlib/cvmorph.cpp       cvlib/cvtables.cpp     \
#cvlib/cxarithm.cpp  cvlib/cxdatastructs.cpp  cvlib/cxsumpixels.cpp   \
#cvlib/cvfilter.cpp    cvlib/cvpyramids.cpp    cvlib/cvthresh.cpp    \
# cvlib/cxarray.cpp   cvlib/cxdrawing.cpp      cvlib/cxtables.cpp \
#cvlib/cvimgwarp.cpp   cvlib/cvshapedescr.cpp  cvlib/cvutils.cpp    \
#  cvlib/cxcmp.cpp    cvlib/cxerror.cpp
