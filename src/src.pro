TEMPLATE = lib

HEADERS += \
           generated/dec_proxy.h \
           generated/socket_proxy.h \
           generated/cache_adaptor.h \
           bridge.h \
           cache.h  \
           struct_marshallers.h \
           constant_mappings.h \
    dbusconnection.h \
    qspiadaptorv2.h

SOURCES += \
           generated/dec_proxy.cpp \
           generated/socket_proxy.cpp \
           generated/cache_adaptor.cpp \
           bridge.cpp \
           cache.cpp  \
           struct_marshallers.cpp \
           constant_mappings.cpp \
           main.cpp \
    dbusconnection.cpp \
    qspiadaptorv2.cpp

QMAKE_CFLAGS+=-Werror
QMAKE_CXXFLAGS+=-Werror

CONFIG += qdbus plugin debug

PKGCONFIG += atspi-2

CONFIG += link_pkgconfig

OBJECTS_DIR = .obj
MOC_DIR = .moc

TARGET = qspiaccessiblebridge
target.path = $$[QT_INSTALL_PLUGINS]/accessiblebridge

INSTALLS += target




