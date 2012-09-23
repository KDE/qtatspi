
set(simple_SRCS
    main.cpp
    mainwindow.cpp
)

kde4_add_ui_files(simple_SRCS
    mainwindow.ui
)

kde4_add_executable(simple ${simple_SRCS})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(simple ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS})
