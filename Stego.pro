#-------------------------------------------------
#
# Project created by QtCreator 2016-09-28T17:18:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

test {

} else {
TARGET = Stego
TEMPLATE = app
}

SOURCES += main.cpp\
        mainwindow.cpp \
    dcstego.cpp \
    dcimage.cpp \
    dccrypto.cpp

HEADERS  += mainwindow.h \
    dcstego.h \
    dcimage.h \
    dccrypto.h

FORMS    += mainwindow.ui
