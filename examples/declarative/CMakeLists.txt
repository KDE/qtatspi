set(declarative_SRCS
    main.cpp
    mainwindow.cpp
)

kde4_add_ui_files(declarative_SRCS
    mainwindow.ui
)

kde4_add_executable(declarative ${declarative_SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES})

target_link_libraries(declarative ${QT_QTCORE_LIBRARY} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KDECORE_LIBS})
