cmake_minimum_required(VERSION 3.16)

project(MotorDriver VERSION 0.1 LANGUAGES CXX)

# Qt otomatik işlemlerini etkinleştir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# C++ standardını ayarla
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PowerShell komutu: LongPathsEnabled değerini kontrol et
execute_process(
    COMMAND powershell -Command "Get-ItemProperty -Path 'HKLM:\\SYSTEM\\CurrentControlSet\\Control\\FileSystem' -Name 'LongPathsEnabled' | Select-Object -ExpandProperty LongPathsEnabled"
    OUTPUT_VARIABLE LONG_PATHS_ENABLED
    ERROR_VARIABLE PS_ERROR
    RESULT_VARIABLE PS_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Sonuç kontrolü
if(${PS_RESULT} EQUAL 0)
    if(${LONG_PATHS_ENABLED} STREQUAL "1")
        message(STATUS "Long Path support is enabled.")
    elseif(${LONG_PATHS_ENABLED} STREQUAL "0")
        message(WARNING "Long Path support is disabled. Please enable it to avoid path length issues.")
    else()
        message(WARNING "Unexpected value for LongPathsEnabled: ${LONG_PATHS_ENABLED}. Please check your system settings.")
    endif()
else()
    message(WARNING "Failed to query LongPathsEnabled. PowerShell error: ${PS_ERROR}")
endif()


include(thirdparty/AdvancedStyleSheet/Advanced-Stylesheet.cmake)
include_directories(thirdparty/QLedIndicator/src)

# Qt6'yı bul
find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Qml PrintSupport StateMachine SerialPort)

# Çeviri dosyalarını ayarla
set(TS_FILES
    translate/MotorDriver_en_GB.ts
    translate/MotorDriver_tr_TR.ts
)

# Proje kaynak dosyalarını tanımla
set(PROJECT_SOURCES
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
    src/ui/DriveSetupAndMotion/DriveSetupAndMotion.cpp
    src/ui/DriveSetupAndMotion/DriveSetupAndMotion.h
    src/ui/DriveSetupAndMotion/DriveSetupAndMotion.ui
)

# Quick Tuning dosyaları
file(GLOB QUICK_TUNING_FILES
    src/ui/DriveSetupAndMotion/QuickTuning/**/*.cpp
    src/ui/DriveSetupAndMotion/QuickTuning/**/*.h
    src/ui/DriveSetupAndMotion/QuickTuning/**/*.ui
)

# Expert Tuning dosyaları
file(GLOB EXPERT_TUNING_FILES
    src/ui/DriveSetupAndMotion/ExpertTuning/**/*.cpp
    src/ui/DriveSetupAndMotion/ExpertTuning/**/*.h
    src/ui/DriveSetupAndMotion/ExpertTuning/**/*.ui
)

file(GLOB MOTION_SINGLE_AXIS_FILES
    src/ui/DriveSetupAndMotion/MotionSingleAxis/**/*.cpp
    src/ui/DriveSetupAndMotion/MotionSingleAxis/**/*.h
    src/ui/DriveSetupAndMotion/MotionSingleAxis/**/*.ui
)

# Single Axis Motion dosyaları
file(GLOB SINGLE_AXIS_FILES
    src/ui/MotionSingleAxis/**/*.cpp
    src/ui/MotionSingleAxis/**/*.h
    src/ui/MotionSingleAxis/**/*.ui
)

# System Configuration dosyaları
file(GLOB SYSTEM_CONFIGURATION_FILES
    src/ui/SystemConfiguration/*.cpp
    src/ui/SystemConfiguration/*.h
    src/ui/SystemConfiguration/*.ui
)

# Common Panel dosyaları
file(GLOB COMMON_PANEL_FILES
    src/ui/CommonPanels/*.cpp
    src/ui/CommonPanels/*.h
    src/ui/CommonPanels/*.ui
)

# Tüm dosyaları birleştir
set(SRC_FILES
    ${MAIN_FILES}
    ${QUICK_TUNING_FILES}
    ${EXPERT_TUNING_FILES}
    ${MOTION_SINGLE_AXIS_FILES}
    ${SINGLE_AXIS_FILES}
    ${SYSTEM_CONFIGURATION_FILES}
    ${COMMON_PANEL_FILES}
)

add_subdirectory(thirdparty/spdlog/)
add_subdirectory(thirdparty/CustomTabToolBarGenerator/)

# Qt6'nın qt_add_executable fonksiyonunu kullanarak yürütülebilir dosyayı ekle
qt_add_executable(MotorDriver
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${SRC_FILES}
    resources/resource.qrc

    src/ui/VerticalMenuBar/VerticalMenuBar.h src/ui/VerticalMenuBar/VerticalMenuBar.cpp src/ui/VerticalMenuBar/VerticalMenuBar.ui
    src/ui/VerticalMenuBar/WorkspaceWidget/WorkspaceWidget.h src/ui/VerticalMenuBar/WorkspaceWidget/WorkspaceWidget.cpp src/ui/VerticalMenuBar/WorkspaceWidget/WorkspaceWidget.ui
    src/ui/VerticalMenuBar/WindowsListWidget/WindowsListWidget.h src/ui/VerticalMenuBar/WindowsListWidget/WindowsListWidget.cpp src/ui/VerticalMenuBar/WindowsListWidget/WindowsListWidget.ui
    src/ui/VerticalMenuBar/MainMenuListWidget/MainMenuListWidget.h src/ui/VerticalMenuBar/MainMenuListWidget/MainMenuListWidget.cpp src/ui/VerticalMenuBar/MainMenuListWidget/MainMenuListWidget.ui
    src/ui/TabToolMenuBar/TabToolMenuBar.h src/ui/TabToolMenuBar/TabToolMenuBar.cpp src/ui/TabToolMenuBar/TabToolMenuBar.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/PositionMode/PositionMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/PositionMode/PositionMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/PositionMode/PositionMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/VelocityMode/VelocityMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/VelocityMode/VelocityMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/VelocityMode/VelocityMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/CurrentMode/CurrentMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/CurrentMode/CurrentMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/CurrentMode/CurrentMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/SineReferenceMode/SineReferenceMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/SineReferenceMode/SineReferenceMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/SineReferenceMode/SineReferenceMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/HomingMode/HomingMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/HomingMode/HomingMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/HomingMode/HomingMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperMode/StepperMode.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperMode/StepperMode.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperMode/StepperMode.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperParameters/StepperParameters.h src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperParameters/StepperParameters.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperParameters/StepperParameters.ui
    src/ui/DriveSetupAndMotion/MotionSingleAxis/MotionSingleAxis.cpp src/ui/DriveSetupAndMotion/MotionSingleAxis/MotionSingleAxis.h src/ui/DriveSetupAndMotion/MotionSingleAxis/MotionSingleAxis.ui
    src/ui/Preferences/Preferences.h src/ui/Preferences/Preferences.cpp src/ui/Preferences/Preferences.ui
    src/ui/Preferences/Settings/SettingsDialog.cpp src/ui/Preferences/Settings/SettingsDialog.h src/ui/Preferences/Settings/SettingsDialog.ui
    src/ui/StateMachine/UIStateMachineController.h src/ui/StateMachine/UIStateMachineController.cpp
    src/ui/StateMachine/SplashScreenState.h src/ui/StateMachine/SplashScreenState.cpp
    src/ui/StateMachine/DisconnectState.h src/ui/StateMachine/DisconnectState.cpp
    src/ui/StateMachine/ConnectedState.h src/ui/StateMachine/ConnectedState.cpp
    src/ui/StateMachine/DriveSetupandMotionState.h src/ui/StateMachine/DriveSetupandMotionState.cpp
    src/ui/StateMachine/SystemConfigurationState.h src/ui/StateMachine/SystemConfigurationState.cpp
    src/ui/StateMachine/DriveSetupandMotionStates/QuickTuningState.h src/ui/StateMachine/DriveSetupandMotionStates/QuickTuningState.cpp
    src/ui/StateMachine/DriveSetupandMotionStates/ExpertTuningState.h src/ui/StateMachine/DriveSetupandMotionStates/ExpertTuningState.cpp
    src/ui/StateMachine/DriveSetupandMotionStates/MotionSingleAxisState.h src/ui/StateMachine/DriveSetupandMotionStates/MotionSingleAxisState.cpp
    src/ui/StateMachine/UIStateMachineContext.h src/ui/StateMachine/UIStateMachineContext.cpp

    src/commands/ICommand.h
    src/commands/LineEditCommand.h src/commands/LineEditCommand.cpp
    src/commands/ComboBoxCommand.h src/commands/ComboBoxCommand.cpp
    src/commands/CheckBoxCommand.h src/commands/CheckBoxCommand.cpp
    src/commands/CommandManager.h src/commands/CommandManager.cpp
    src/commands/RadioButtonCommand.h src/commands/RadioButtonCommand.cpp
    src/commands/SpinBoxCommand.h src/commands/SpinBoxCommand.cpp
    src/commands/SliderCommand.h src/commands/SliderCommand.cpp

    src/utils/Utils.h src/utils/Utils.cpp
    src/utils/Constants.h
    src/utils/GuardedSignalTransition.h

    src/controllers/ApplicationController.h src/controllers/ApplicationController.cpp
    src/controllers/VerticalMenuBar/VerticalMenuBarController.h src/controllers/VerticalMenuBar/VerticalMenuBarController.cpp
    src/controllers/DriveSetupAndMotion/DriveSetupAndMotionController.h src/controllers/DriveSetupAndMotion/DriveSetupAndMotionController.cpp
    src/controllers/TabToolMenuBar/TabToolMenuBarController.h src/controllers/TabToolMenuBar/TabToolMenuBarController.cpp
    src/controllers/Preferences/PreferencesController.h src/controllers/Preferences/PreferencesController.cpp
    src/controllers/StateMachine/BusinessStateMachineController.h src/controllers/StateMachine/BusinessStateMachineController.cpp

    src/events/EventAggregator.h src/events/EventAggregator.cpp
    src/events/EventAggregator.h src/events/EventAggregator.cpp


    src/core/MotorDriver/IMotorDriver.h
    src/core/MotorDriver/GoldDriveMotor.h src/core/MotorDriver/GoldDriveMotor.cpp
    src/core/MotorDriver/PlatiniumDriveMotor.h src/core/MotorDriver/PlatiniumDriveMotor.cpp
    src/core/MotorDriver/MotorDriverFactory.h
    src/core/MotorDriver/MotorDriverManager.h
    src/core/ErrorHandling/ErrorHandling.h src/core/ErrorHandling/ErrorHandling.cpp src/core/ErrorHandling/ErrorHandling.ui
    src/core/LogManager/LogManager.h src/core/LogManager/LogManager.cpp
    src/core/ChartManager/ChartController.h src/core/ChartManager/ChartController.cpp

    src/settings/SettingsManager.h
    src/controllers/DriveSetupAndMotion/QuickTuning/AxisConfiguration/AxisConfigurationController.h src/controllers/DriveSetupAndMotion/QuickTuning/AxisConfiguration/AxisConfigurationController.cpp
    src/controllers/StateMachine/ConnectionStates/ConnectedControllerState.h src/controllers/StateMachine/ConnectionStates/ConnectedControllerState.cpp
    src/controllers/StateMachine/ConnectionStates/DisconnectControllerState.h src/controllers/StateMachine/ConnectionStates/DisconnectControllerState.cpp
    src/controllers/StateMachine/SystemConfigurationStates/SystemConfigurationControllerState.h src/controllers/StateMachine/SystemConfigurationStates/SystemConfigurationControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/DriveSetupAndMotionControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/DriveSetupAndMotionControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/AxisConfiguration/AxisConfigurationsControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/AxisConfiguration/AxisConfigurationsControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/FeedBackSettingsControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/FeedBackSettingsControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/MotorSettingsControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/MotorSettingsControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/ApplicationSettings/BrakeControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/ApplicationSettings/BrakeControllerState.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/AutomaticTuning/AutomaticTuningControllerState.h src/controllers/StateMachine/DriveSetupAndMotion/QuickTuning/AutomaticTuning/AutomaticTuningControllerState.cpp
    src/controllers/DriveSetupAndMotion/QuickTuning/ApplicationSettings/BrakeController.h src/controllers/DriveSetupAndMotion/QuickTuning/ApplicationSettings/BrakeController.cpp
    src/controllers/DriveSetupAndMotion/QuickTuning/AutomaticTuning/AutomaticTuningController.h src/controllers/DriveSetupAndMotion/QuickTuning/AutomaticTuning/AutomaticTuningController.cpp
    src/controllers/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/FeedBackSettingsController.h src/controllers/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/FeedBackSettingsController.cpp
    src/controllers/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/MotorSettingsController.h src/controllers/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack/MotorSettingsController.cpp
    src/controllers/DriveSetupAndMotion/QuickTuning/QuickTuningController.h src/controllers/DriveSetupAndMotion/QuickTuning/QuickTuningController.cpp
    src/controllers/StateMachine/BusinessStateMachineContext.h src/controllers/StateMachine/BusinessStateMachineContext.cpp
    src/core/CommunicationManager/CommunicationManager.h src/core/CommunicationManager/CommunicationManager.cpp
    src/core/CommunicationManager/Channels/ICommunicationChannel.h
    src/core/CommunicationManager/Channels/SerialPortChannel.h src/core/CommunicationManager/Channels/SerialPortChannel.cpp
    src/core/CommunicationManager/Protocol/IProtocol.h
    src/core/CommunicationManager/Protocol/SimpleIQProtocol.h src/core/CommunicationManager/Protocol/SimpleIQProtocol.cpp
    src/core/CommunicationManager/Model/Command.h src/core/CommunicationManager/Model/Command.cpp
    src/core/CommunicationManager/Model/Response.h src/core/CommunicationManager/Model/Response.cpp
    src/core/CommunicationManager/Utils/CircularBuffer.h src/core/CommunicationManager/Utils/CircularBuffer.cpp
    src/core/CommunicationManager/Utils/MessageQueue.h src/core/CommunicationManager/Utils/MessageQueue.cpp
    test/TestCommunicationWidget/TestCommunicationWidget.h test/TestCommunicationWidget/TestCommunicationWidget.cpp
    src/controllers/BaseController/AbstractController.h src/controllers/BaseController/AbstractController.cpp
    src/core/CommandDispatcher/CommandDispatcher.h src/core/CommandDispatcher/CommandDispatcher.cpp
    src/controllers/DriveSetupAndMotion/ExpertTuning/Current/CurrentIdentificationController.h src/controllers/DriveSetupAndMotion/ExpertTuning/Current/CurrentIdentificationController.cpp
    src/controllers/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsController.h src/controllers/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsController.cpp
    src/controllers/StateMachine/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsState.h src/controllers/StateMachine/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections/CurrentLimitsState.cpp
    src/controllers/StateMachine/Communication/StateCommunicationManager.h
    src/core/CommandDispatcher/SequentialCommandProcessor.h src/core/CommandDispatcher/SequentialCommandProcessor.cpp
    src/core/CommandDispatcher/PeriodicQueryManager.h src/core/CommandDispatcher/PeriodicQueryManager.cpp
)

qt_add_translations(MotorDriver TRANSLATIONS  ${TS_FILES})

# Include dizinlerini belirle
target_include_directories(MotorDriver PRIVATE
    src/settings
    src/events
    src/commands
    src/utils
    src/ui

    src/ui/DriveSetupAndMotion/QuickTuning/AxisConfiguration
    src/ui/CommonPanels
    src/ui/DriveSetupAndMotion/QuickTuning/ApplicationSettings
    src/ui/DriveSetupAndMotion/QuickTuning/MotorAndFeedBack
    src/ui/DriveSetupAndMotion/QuickTuning/AutomaticTuning
    src/ui/DriveSetupAndMotion/ExpertTuning/AplicationSettings
    src/ui/DriveSetupAndMotion/ExpertTuning/Commutation
    src/ui/DriveSetupAndMotion/ExpertTuning/Current
    src/ui/DriveSetupAndMotion/ExpertTuning/FunctionalSafety
    src/ui/DriveSetupAndMotion/ExpertTuning/GantryYaw
    src/ui/DriveSetupAndMotion/ExpertTuning/LimitsAndProtections
    src/ui/DriveSetupAndMotion/ExpertTuning/Safety
    src/ui/DriveSetupAndMotion/ExpertTuning/SafetyIOConfiguration
    src/ui/DriveSetupAndMotion/ExpertTuning/StepperClosedLoop
    src/ui/DriveSetupAndMotion/ExpertTuning/Summary
    src/ui/DriveSetupAndMotion/ExpertTuning/UserUnits
    src/ui/DriveSetupAndMotion/ExpertTuning/VelocityAndPosition
    src/ui/VerticalMenuBar/MainMenuListWidget
    src/ui/VerticalMenuBar/WindowsListWidget
    src/ui/VerticalMenuBar/WorkspaceWidget
    src/ui/TabToolMenuBar
    src/ui/DriveSetupAndMotion
    src/ui/VerticalMenuBar
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/CurrentMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/HomingMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/PositionMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/SineReferenceMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/StepperParameters
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion/DriveModePages/VelocityMode
    src/ui/DriveSetupAndMotion/MotionSingleAxis/SingleAxisMotion
    src/ui/DriveSetupAndMotion/MotionSingleAxis/Recorder
    src/ui/DriveSetupAndMotion/MotionSingleAxis/
    src/ui/StateMachine/DriveSetupandMotionStates
    src/ui/Preferences
    src/ui/SystemConfiguration
    src/ui/StateMachine

    include
    include/Qwt

    src/core/MotorDriver
    src/core/ErrorHandling
    src/core/LogManager
    src/core/ChartManager

    src/controllers/StateMachine
    src/controllers
    src/controllers/VerticalMenuBar
    src/controllers/DriveSetupAndMotion
    src/controllers/TabToolMenuBar
    src/controllers/Preferences
    src/controllers/DriveSetupAndMotion/QuickTuning/AxisConfiguration
    src/controllers/StateMachine/ConnectionStates
    src/controllers/DriveSetupAndMotion/ExpertTuning/Current
)

# Qt6 bileşenlerini hedefe bağla
target_link_libraries(MotorDriver PRIVATE Qt6::Widgets
    ${CMAKE_SOURCE_DIR}/libs/ExpanderWidget.dll
    ${CMAKE_SOURCE_DIR}/libs/qwt.dll
    ${CMAKE_SOURCE_DIR}/libs/qwtd.dll
    spdlog::spdlog
    TabToolbar
    ${CMAKE_SOURCE_DIR}/libs/libqtadvancedcss.dll
    ${CMAKE_SOURCE_DIR}/libs/libQLedIndicator.dll
    Qt6::Qml
    Qt6::PrintSupport
    Qt6::StateMachine
    Qt6::SerialPort
)

# C++20 özelliklerini etkinleştir
target_compile_features(MotorDriver PRIVATE cxx_std_20)

# Derleme optimizasyonu
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Qt'nin Release modunda debug bilgilerini devre dışı bırak
    target_compile_definitions(MotorDriver PRIVATE QT_NO_DEBUG_OUTPUT)

    # C++20 coroutines için destek
    target_compile_options(MotorDriver PRIVATE -fcoroutines)

    # Yüksek optimize edilmiş derleme ayarları
    if(MSVC)
        target_compile_options(MotorDriver PRIVATE /O2)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()
endif()

# Test konfigürasyonu (isteğe bağlı)
option(BUILD_TESTS "Build the tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Yürütülebilir dosyayı sonlandır
qt_finalize_executable(MotorDriver)
