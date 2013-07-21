#-------------------------------------------------
#
# Project created by QtCreator 2013-07-21T21:10:18
#
#-------------------------------------------------

QT       -= core gui

TARGET = bmpbox
TEMPLATE = lib
CONFIG += staticlib

header_files.files += bmpbox.h \
                      bmp.h


SOURCES += bmpbox.cpp

HEADERS += bmpbox.h \
    bmp.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
        header_files.path = /opt/usr/include/bmpbox/
    } else {
        target.path = /usr/lib
        header_files.path = /usr/include/bmpbox/
    }
    INSTALLS += target \
                header_files
}