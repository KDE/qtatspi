TEMPLATE = lib

HEADERS += \
           generated/marshallers.h \
           generated/dec_proxy.h \
           generated/socket_proxy.h \
           generated/accessible_adaptor.h \
           generated/action_adaptor.h \
           generated/application_adaptor.h \
           generated/cache_adaptor.h \
           generated/component_adaptor.h \
           generated/editable_text_adaptor.h \
           generated/event_adaptor.h \
           generated/table_adaptor.h \
           generated/text_adaptor.h \
           generated/value_adaptor.h \
           application.h \
           bridge.h \
           cache.h  \
           adaptor.h \
           struct_marshallers.h \
           constant_mappings.h \
           accessible.h \
    dbusconnection.h \
    qspiadaptorv2.h

SOURCES += \
           generated/dec_proxy.cpp \
           generated/socket_proxy.cpp \
           generated/accessible_adaptor.cpp \
           generated/action_adaptor.cpp \
           generated/application_adaptor.cpp \
           generated/cache_adaptor.cpp \
           generated/component_adaptor.cpp \
           generated/editable_text_adaptor.cpp \
           generated/event_adaptor.cpp \
           generated/table_adaptor.cpp \
           generated/text_adaptor.cpp \
           generated/value_adaptor.cpp \
           application.cpp \
           bridge.cpp \
           cache.cpp  \
           adaptor.cpp \
           struct_marshallers.cpp \
           constant_mappings.cpp \
           accessible.cpp \
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


