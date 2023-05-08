QT       += core gui
QT       += multimedia
QT       += network
QT       -= pdf
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += static
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    desktoplyrics.cpp \
    fileutils.cpp \
    lyricparser.cpp \
    main.cpp \
    musicscraper.cpp \
    musiczz123.cpp \
    searchwidget.cpp

HEADERS += \
    desktoplyrics.h \
    fileutils.h \
    lyricparser.h \
    musicscraper.h \
    musiczz123.h \
    searchwidget.h

macx: {
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15 # 或者更高的版本
  QMAKE_INFO_PLIST = Info.plist
  ICON = asserts/music.icns
#  LIBS += -F/Users/and1zx/Qt/6.5.0/macos/lib
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    asserts.qrc
