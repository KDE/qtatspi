
set(calculator_SRCS
    main.cpp
    calculator.cpp
    button.cpp
)

kde4_add_executable(calculator ${calculator_SRCS})

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(calculator ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
