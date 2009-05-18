TEMPLATE = lib

HEADERS += cache.h  \
           object.h \
           application.h \
           application_adaptor.h \
           component_adaptor.h \
           action_adaptor.h \
           value_adaptor.h \
           proxy_marshallers.h \
           constant_mappings.h \
           tree_adaptor.h \
           text_adaptor.h \
           editable_text_adaptor.h \
           accessible_adaptor.h \
           registry_proxy.h


SOURCES += bridge.cpp \
           cache.cpp  \
           object.cpp \
           application.cpp \
           application_adaptor_impl.cpp \
           component_adaptor_impl.cpp \
           action_adaptor_impl.cpp \
           value_adaptor_impl.cpp \
           proxy_marshallers.cpp \
           constant_mappings.cpp \
           tree_adaptor_impl.cpp \
           text_adaptor_impl.cpp \
           editable_text_adaptor_impl.cpp \
           adaptor_marshallers.cpp \
           accessible_adaptor_impl.cpp \
           registry_proxy.cpp

CONFIG += qdbus plugin debug accessibility

DEFINES += QT_ACCESSIBILITY

OBJECTS_DIR = tmp
MOC_DIR = tmp

TARGET = qspiaccessiblebridge
target.path = $$[QT_INSTALL_PLUGINS]/accessiblebridge

INSTALLS += target
