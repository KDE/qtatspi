
SOURCES += tst_qt-atspi.cpp \
    ../src/struct_marshallers.cpp

CONFIG += gui qtestlib qdbus
TEMPLATE = app
TARGET =  tst_qt-atspi
DEPENDPATH += .
INCLUDEPATH += .
