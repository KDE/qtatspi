TEMPLATE = lib

HEADERS += cache.h  \
           object.h \
           proxy_marshallers.h \
           constant_mappings.h \
           tree_adaptor.h \
           accessible_adaptor.h \
           registry_proxy.h


SOURCES += bridge.cpp \
           cache.cpp  \
           object.cpp \
           proxy_marshallers.cpp \
           constant_mappings.cpp \
           tree_adaptor_impl.cpp \
           adaptor_marshallers.cpp \
           accessible_adaptor_impl.cpp \
           registry_proxy.cpp

CONFIG += qdbus plugin debug

OBJECTS_DIR = tmp
MOC_DIR = tmp

TARGET = qspiaccessiblebridge
target.path = $$[QT_INSTALL_PLUGINS]/accessiblebridge

INSTALLS += target
