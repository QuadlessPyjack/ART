TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG += qt

QT += core
QT += qml quick

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
    frameprocessor.cpp \
    patternsolver.cpp \
    qstreamintegratedview.cpp


win32 {

    INCLUDEPATH += $$(OPENCV_DIR)/include
    INCLUDEPATH += C:/Aruco/include

    CONFIG(debug, debug|release) {
        LIBS += C:/OpenCV/x64/vc15/lib/debug/opencv_*.lib
        LIBS += C:/Aruco/lib/aruco3013.lib
    } else {
        LIBS += C:/OpenCV/x64/vc15/lib/release/opencv_*.lib
        LIBS += C:/Aruco/lib/aruco3013.lib
    }
}

linux {
    INCLUDEPATH += /usr/local/include/opencv4
    INCLUDEPATH += /usr/local/include/aruco

        LIBS += -lopencv_core \
                -lopencv_imgproc \
                -lopencv_imgcodecs \
                -lopencv_video \
                -lopencv_videoio \
                -lopencv_highgui \
                -laruco

#    CONFIG(debug, debug|release) {
#        LIBS += -lopencv_core \
#                -lopencv_imgproc \
#                -lopencv_video \
#                -lopencv_highgui \
#                -laruco
#    } else {
#        LIBS += -lopencv_core \
#                -lopencv_imgproc \
#                -lopencv_video \
#                -lopencv_highgui \
#                -laruco
#    }
}

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

DISTFILES += \
    qtquickcontrols2.conf

HEADERS += \
    frameprocessor.h \
    patternsolver.h \
    qstreamintegratedview.h \
    utils.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libBLECentral/release/ -llibBLECentral
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libBLECentral/debug/ -llibBLECentral
else:unix:!macx: LIBS += -L$$OUT_PWD/../libBLECentral/ -llibBLECentral

INCLUDEPATH += $$PWD/../libBLECentral
DEPENDPATH += $$PWD/../libBLECentral
