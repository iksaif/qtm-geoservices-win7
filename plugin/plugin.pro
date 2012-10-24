TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtposition_win7)
PLUGIN_TYPE=position

include(../common.pri)

LIB = locationapi.lib

WINSDK = $$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\)

INCLUDEPATH += $$quote($${WINSDK}\Include\)
LIBS += -L$$quote($${WINSDK}\Lib\) locationapi.lib propsys.lib

DEFINES = Q_LOCATION_WIN7_DEBUG

CONFIG += mobility
MOBILITY = location

HEADERS +=  qgeopositioninfosource_win7.h \
            qgeopositioninfosourcefactory_win7.h \
    qgeolocationevents_win7_p.h

SOURCES +=  qgeopositioninfosource_win7.cpp \
            qgeopositioninfosourcefactory_win7.cpp \
    qgeolocationevents_win7.cpp


