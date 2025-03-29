cmake_minimum_required(VERSION 3.5)

project(Advanced-StyleSheet-Cmake-Example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(STYLES_DIR "C:/Users/aakdeniz/Desktop/MyProject/QSS/Advanced-StyleSheet-Cmake-Example/styles")

add_definitions(-DSTYLES_DIR=${STYLES_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg QuickWidgets )
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg QuickWidgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/emir/include)
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        # QtAdvancedStylesheet.cpp
        # QtAdvancedStylesheet.h
        # acss_globals.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Advanced-StyleSheet-Cmake-Example
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Advanced-StyleSheet-Cmake-Example APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Advanced-StyleSheet-Cmake-Example SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        message(WARNING "!!!!!!!!!!!!!!!!!!!!!")
        add_executable(Advanced-StyleSheet-Cmake-Example
            ${PROJECT_SOURCES}
        )
    endif()
endif()


target_link_libraries(Advanced-StyleSheet-Cmake-Example PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    ${CMAKE_SOURCE_DIR}/emir/lib/libqtadvancedcss.dll
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Advanced-StyleSheet-Cmake-Example)
endif()
set_target_properties(Advanced-StyleSheet-Cmake-Example PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Advanced-StyleSheet-Cmake-Example
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Advanced-StyleSheet-Cmake-Example)
endif()
