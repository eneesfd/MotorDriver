
set(QT_ADVANCED_CSS_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

message(STATUS "AdvancedStylesheet include directory: ${QT_ADVANCED_CSS_INCLUDE_DIR}")

message(STATUS "Qt ${LIB_QT_VERSION} to use .")

set(QT_ADVANCED_CSS_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/Qt6)

message(STATUS "Advanced-Stylesheet library directory: ${QT_ADVANCED_CSS_LIB_DIR}/libqtadvancedcss.dll")

set(QT_ADVANCED_CSS_LIB ${QT_ADVANCED_CSS_LIB_DIR}/libqtadvancedcss.dll)

message(STATUS "Advanced-Stylesheet library: ${QT_ADVANCED_CSS_LIB}")
file(COPY ${CMAKE_CURRENT_LIST_DIR}/styles DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
