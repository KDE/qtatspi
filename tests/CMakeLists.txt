
set(tst_qt-atspi_SRCS
    tst_qt-atspi.cpp
    ${CMAKE_SOURCE_DIR}/src/dbusconnection.cpp
    ${CMAKE_SOURCE_DIR}/src/struct_marshallers.cpp
    ${CMAKE_SOURCE_DIR}/
)

include_directories(${CMAKE_BINARY_DIR}/tests)
message("Building test...")
message(${CMAKE_BINARY_DIR})

kde4_add_unit_test(tst_qt-atspi ${tst_qt-atspi_SRCS})
target_link_libraries(tst_qt-atspi ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTDBUS_LIBRARY} ${X11_X11_LIB})
