
HEADERS += \
    ../src/dbusconnection.h \
    ../src/struct_marshallers.h

SOURCES += tst_qt-atspi.cpp \
    ../src/dbusconnection.cpp \
    ../src/struct_marshallers.cpp

CONFIG += gui qtestlib qdbus
LIBS += -lX11

TEMPLATE = app
TARGET =  tst_qt-atspi

