cmake_minimum_required(VERSION 3.16)
project(TestProject VERSION 1.0 LANGUAGES CXX)

# Qt otomatik işlemlerini etkinleştir
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Qt6 paketlerini bul
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Kaynak dizinini CMAKE_CURRENT_SOURCE_DIR olarak ayarla
set(SRCROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Kaynak dosyalarını listele
set(SRC_LIST
    ${SRCROOT}/main.cpp
    ${SRCROOT}/mainwindow.cpp
    ${SRCROOT}/mainwindow.h
)

# UI dosyalarını listele
set(UIS
    ${SRCROOT}/mainwindow.ui
)

# Resource dosyalarını listele
set(RES
    ${SRCROOT}/res.qrc
)

# Uygulamayı ekle
add_executable(Test
    ${SRC_LIST}
    ${UIS}
    ${RES}
)

# Gerekli kütüphaneleri bağla
target_link_libraries(Test
    TabToolbar
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Hedef özelliklerini ayarla
set_target_properties(Test PROPERTIES DEBUG_POSTFIX d)
set_target_properties(Test PROPERTIES FOLDER "Examples")

# Kurulum ayarları
install(TARGETS Test
        RUNTIME DESTINATION ${INSTALL_MISC_DIR}/examples/Test COMPONENT examples
        BUNDLE DESTINATION ${INSTALL_MISC_DIR}/examples/Test COMPONENT examples)

install(FILES ${SRC_LIST} ${UIS} ${RES}
        DESTINATION ${INSTALL_MISC_DIR}/examples/Test
        COMPONENT examples)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons"
        DESTINATION ${INSTALL_MISC_DIR}/examples/Test
        COMPONENT examples)
