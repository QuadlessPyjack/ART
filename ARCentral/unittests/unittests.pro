QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

HEADERS += \
            ..\atom\payload.h \
    utils/circularbuffer.h \
    utils/circularqueue.h \
    utils/payload.h \
    consts.h

SOURCES +=  tst_utils.cpp \
    circularbuffer.cpp \
    circularqueue.cpp
