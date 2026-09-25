cmake_minimum_required(VERSION 3.19)
project(CustomCefBrowser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point this to your CEF binary distribution directory
set(CEF_ROOT "" CACHE PATH "Path to CEF root directory")

if(NOT CEF_ROOT)
    message(WARNING "CEF_ROOT is not set. Please download CEF binary distribution from https://cef-builds.spotifycdn.com/index.html and pass -DCEF_ROOT=/path/to/cef")
endif()

if(EXISTS "${CEF_ROOT}/cmake")
    list(APPEND CMAKE_MODULE_PATH "${CEF_ROOT}/cmake")
    find_package(CEF REQUIRED)
    include_directories(${CEF_INCLUDE_DIRS})

    add_executable(custom_cef_browser
        main.cpp
        client_app.cpp
        client_handler.cpp
        custom_v8_handler.cpp
    )

    add_dependencies(custom_cef_browser libcef_dll_wrapper)
    target_link_libraries(custom_cef_browser libcef_dll_wrapper ${CEF_LIBRARIES})
else()
    message(STATUS "CEF CMake module not found in CEF_ROOT. Template files provided for reference.")
endif()
