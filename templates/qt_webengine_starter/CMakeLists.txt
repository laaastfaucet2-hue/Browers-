cmake_minimum_required(VERSION 3.16)
project(CustomQtWebEngineBrowser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets WebEngineCore WebEngineWidgets QUIET)

if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Widgets WebEngineCore WebEngineWidgets REQUIRED)
    set(QT_LIBRARIES Qt5::Widgets Qt5::WebEngineWidgets)
else()
    set(QT_LIBRARIES Qt6::Widgets Qt6::WebEngineWidgets)
endif()

add_executable(custom_qt_browser
    main.cpp
    browser_window.cpp
    custom_request_interceptor.cpp
    custom_scheme_handler.cpp
)

target_link_libraries(custom_qt_browser PRIVATE ${QT_LIBRARIES})
