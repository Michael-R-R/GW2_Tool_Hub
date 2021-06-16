QT       += core gui
QT       += network
QT       += sql
QT       += webenginewidgets
QT       += widgets


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += c++14
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    NonWidgetsCpp/DataInterface.cpp \
    NonWidgetsCpp/ErrorHandling.cpp \
    WidgetsCpp/Notes/Notes.cpp \
    NonWidgetsCpp/SaveAndLoad.cpp \
    NonWidgetsCpp/ApiAccess.cpp \
    WidgetsCpp/MaterialTracker/MaterialStatusBar.cpp \
    WidgetsCpp/MainWindow/MainWindow.cpp \
    WidgetsCpp/Browser/Browser.cpp \
    NonWidgetsCpp/main.cpp \
    WidgetsCpp/MaterialTracker/MaterialTracker.cpp \
    WidgetsCpp/MaterialTracker/Materials.cpp



HEADERS += \
    NonWidgetsHeader/DataInterface.h \
    NonWidgetsHeader/ErrorHandling.h \
    WidgetsHeader/Notes/Notes.h \
    NonWidgetsHeader/SaveAndLoad.h \
    NonWidgetsHeader/ApiAccess.h \
    WidgetsHeader/MaterialTracker/MaterialStatusBar.h \
    WidgetsHeader/MainWindow/MainWindow.h \
    WidgetsHeader/Browser/Browser.h \
    WidgetsHeader/MaterialTracker/MaterialTracker.h \
    WidgetsHeader/MaterialTracker/Materials.h


FORMS += \
    Forms/Browser.ui \
    Forms/MainWindow.ui \
    Forms/MaterialStatusBar.ui \
    Forms/MaterialTracker.ui \
    Forms/Materials.ui \
    Forms/Notes.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
