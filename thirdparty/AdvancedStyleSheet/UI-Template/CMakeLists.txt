# Set minimum required version of cmake
cmake_minimum_required(VERSION 3.5)

# Set project name and version
project(qtadvancedcss VERSION 1.0.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Check for build type (debug/release)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find required Qt packages
find_package(Qt5 COMPONENTS Core Gui Widgets Qml Svg QuickWidgets REQUIRED)

# Define a variable for sources
set(SOURCES
    QmlStyleUrlInterceptor.cpp
    QtAdvancedStylesheet.cpp
    # mainwindow.cpp
    # mainwindow.h
    # mainwindow.ui
)

# Define a variable for headers
set(HEADERS
    # mainwindow.h
    acss_globals.h
    QmlStyleUrlInterceptor.h
    QtAdvancedStylesheet.h
)

# Define the library
add_library(qtadvancedcss ${SOURCES} ${HEADERS})

# Specify include directories
target_include_directories(qtadvancedcss PRIVATE ${CMAKE_SOURCE_DIR})

# Link Qt libraries
target_link_libraries(qtadvancedcss Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Qml Qt5::Svg Qt5::QuickWidgets)

# Compiler options
if(WIN32)
    if(MINGW)
        target_compile_options(qtadvancedcss PRIVATE -Wall -Wextra -pedantic)
    endif()
    if(MSVC)
        target_compile_options(qtadvancedcss PRIVATE /utf-8)
    endif()
endif()

# Installation settings
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "../installed" CACHE PATH "..." FORCE)
    message(WARNING "Install Prefix not set. Defaulting to ${CMAKE_INSTALL_PREFIX}")
endif()

install(TARGETS qtadvancedcss DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# Additional settings for build configurations
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
if(BUILD_SHARED_LIBS)
    target_compile_definitions(qtadvancedcss PRIVATE ACSS_SHARED_EXPORT)
else()
    target_compile_definitions(qtadvancedcss PRIVATE ACSS_STATIC)
endif()



