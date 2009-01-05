TEMPLATE = lib

HEADERS += registry-proxy.h

SOURCES += registry-proxy.cpp \
           bridge.cpp

CONFIG += qdbus plugin

OBJECTS_DIR = tmp
MOC_DIR = tmp

DESTDIR = $$[QT_INSTALL_PLUGINS]/accessiblebridge
TARGET = qspiadaptor
