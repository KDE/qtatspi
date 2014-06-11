
project(qspiaccessiblebridge)

find_package(X11 REQUIRED)

#add_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

set(qspiaccessiblebridge_SRCS
           generated/dec_proxy.cpp
           generated/socket_proxy.cpp
           generated/cache_adaptor.cpp
           application.cpp
           bridge.cpp
           cache.cpp
           struct_marshallers.cpp
           constant_mappings.cpp
           main.cpp
           dbusconnection.cpp
           standardactionwrapper.cpp
           atspiadaptor.cpp
)

# when building the files in src/generated we need them to find the files in src since
# headers are needed from there for the types used in dbus
include_directories(${CMAKE_SOURCE_DIR}/src)

kde4_add_library(qspiaccessiblebridge SHARED ${qspiaccessiblebridge_SRCS})

target_link_libraries(qspiaccessiblebridge ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${X11_X11_LIB} ${KDE4_KDECORE_LIBS})

install(TARGETS qspiaccessiblebridge DESTINATION ${QTATSPI_QT_PLUGIN_INSTALL_DIR}/accessiblebridge/)

