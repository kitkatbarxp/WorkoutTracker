#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T14:07:50
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WorkoutTracker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    datamanipulation.cpp \
    routine.cpp

HEADERS  += mainwindow.h \
    datamanipulation.h \
    routine.h

OTHER_FILES += \
    ../../../../../../../../AppData/Local/WorkoutTracker/data.xml

RESOURCES +=
