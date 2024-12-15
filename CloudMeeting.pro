QT       += core gui multimedia multimediawidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


win32-msvc*:QMAKE_CXXFLAGS += /wd"4819" /utf-8

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += $$PWD/depend/include
INCLUDEPATH += $$PWD/depend/dependencies2019/win64/include
INCLUDEPATH += $$PWD/include
LIBS += -L$$PWD/depend/lib -lobs

SOURCES += \
    ./src/Menu/AudioInput.cpp \
    ./src/Menu/AudioOutput.cpp \
    ./src/Menu/chatmessage.cpp \
    ./src/Menu/logqueue.cpp \
    ./src/Menu/main.cpp \
    ./src/Menu/mytcpsocket.cpp \
    ./src/Menu/mytextedit.cpp \
    ./src/Menu/myvideosurface.cpp \
    ./src/Menu/netheader.cpp \
    ./src/Menu/partner.cpp \
    ./src/Menu/recvsolve.cpp \
    ./src/Menu/screen.cpp \
    ./src/Menu/sendimg.cpp \
    ./src/Menu/sendtext.cpp \
    ./src/Menu/widget.cpp \
    ./src/sourceCatch/CTitleBar.cpp \
    ./src/sourceCatch/DesktopRec.cpp \
    ./src/sourceCatch/ObsWrapper.cpp
HEADERS += \
    ./include/AudioInput.h \
    ./include/AudioOutput.h \
    ./include/chatmessage.h \
    ./include/logqueue.h \
    ./include/mytcpsocket.h \
    ./include/mytextedit.h \
    ./include/myvideosurface.h \
    ./include/netheader.h \
    ./include/netheader.h \
    ./include/partner.h \
    ./include/recvsolve.h \
    ./include/screen.h \
    ./include/sendimg.h \
    ./include/sendtext.h \
    ./include/widget.h \
	./include/CTitleBar.h \
	./include/DesktopRec.h \
	./include/ObsWrapper.h 

FORMS += \
    ./ui/CTitleBar.ui \
	./ui/DesktopRec.ui \
    ./ui/widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc \
