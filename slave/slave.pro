QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

INCLUDEPATH += "/usr/local/qwt-6.1.2/include"
LIBS += -L”/usr/local/qwt-6.1.2/lib/” -lqwt

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    comm.cpp \
    main.cpp \
    monitor.cpp \
    slave.cpp \
    util.cpp

HEADERS += \
    camera.h \
    comm.h \
    monitor.h \
    slave.h \
    util.h

FORMS += \
    slave.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
