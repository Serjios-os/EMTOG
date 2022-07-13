QT       += core gui
QT       += quick
QT       += sql
#win*:     QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    blockmanager.cpp \
    dashboard.cpp \
    databasemanager.cpp \
    datacalculator.cpp \
    datavalidator.cpp \
    datawrapper.cpp \
    equipmentsmanager.cpp \
    main.cpp \
    elecmtugen.cpp \
    projectmanager.cpp

HEADERS += \
    blockmanager.h \
    configuration.h \
    dashboard.h \
    databasemanager.h \
    datacalculator.h \
    datavalidator.h \
    datawrapper.h \
    elecmtugen.h \
    equipmentsmanager.h \
    projectmanager.h

FORMS += \
    dashboard.ui \
    elecmtugen.ui \
    equipmentsmanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    ressource/ressource.qrc

RC_ICONS    += \
    ressource/icon.ico
