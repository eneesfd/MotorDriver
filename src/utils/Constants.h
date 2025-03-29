#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstddef>
#include <QFile>
#include <QEvent>
#include <QColor>

namespace Constants {
namespace Log {
constexpr int ROTATING_LIMIT = 5;
constexpr size_t ROTATING_FILE_COUNT = 50 * 1024 * 1024; // 50 MB
constexpr const char* LOG_DIR_STR = "C:\\DevTools\\070.41-Yeni-Servo\\build\\Desktop_Qt_6_8_1_MinGW_64_bit-Debug\\log";
const std::filesystem::path LOG_DIR = LOG_DIR_STR;
enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    OFF
};
}
namespace DriveSetupAndMotionContants {

namespace QuickTuningConstants  {
enum class QuickTuningPages {
    AXIS_CONFIGURATIONS = 0,

    MOTOR_SETTINGS = 3,
    FEEDBACK_SETTINGS = 4,

    BRAKE = 11,

    AUTOMATIC_TUNING = 29,
};

inline QuickTuningPages fromLogicalIndex(int logicalIndex) {
    switch (logicalIndex) {
    case 0: return QuickTuningPages::AXIS_CONFIGURATIONS;
    case 1: return QuickTuningPages::MOTOR_SETTINGS;
    case 2: return QuickTuningPages::FEEDBACK_SETTINGS;
    case 3: return QuickTuningPages::BRAKE;
    case 4: return QuickTuningPages::AUTOMATIC_TUNING;
    default: return QuickTuningPages::AXIS_CONFIGURATIONS; // Varsayılan
    }
}

}

namespace ExpertTuningConstants {
enum class ExpertTuningPages {
    AXIS_CONFIGURATIONS = 0,
    FUNTIONAL_SAFETY,
    SAFETY_IO_CONFIGURATION,
    MOTOR_SETTINGS,
    FEEDBACK_SETTINGS,

    DRIVE_USER_UNITS,
    DISPLAY_USER_UNITS,

    CURRENT_LIMITS,
    MOTION_LIMITS_AND_MUDOLO,
    PROTECTIONS,
    STOPPING_OPTIONS,

    BRAKE,
    SETTING_WINDOWS,
    INPUTS_AND_OUTPUTS,

    CURRENT_IDENTIFICATION,
    CURRENT_DESIGN,
    CURRENT_VERIFICATION_TIME,

    COMMUTATION,
    STOPPER_CLOSED_LOOP,

    GANTRY_YAW_IDENTIFICATION,
    GANTRY_YAW_DESIGN,

    VELOCITY_AND_POSITION_IDENTIFICATION,
    VELOCITY_AND_POSITION_DESIGN,
    VELOCITY_AND_POSITION_SCHEDULING,
    VELOCITY_AND_POSITION_VERIFICATION_TIME,

    ENABLING_DEVICE,
    VALIDATION,
    ACCEPTANCE,
    SUMMARY,
    AUTOMATIC_TUNING,
};
}

enum class TuningMode {
    QUICK_TUNING,
    EXPERT_TUNING
};
}

namespace VerticalMenuBarConstants {
enum class WindowsListPages {
    QUICK_TUNING = 0,
    EXPERT_TUNING,
    MOTION_SINGLE_AXIS
};
enum class MainMenuPages {
    SYSTEM_CONFIGURATION = 0,
    DRIVE_SETUP_AND_MOTION,
    QUICK_TUNING,
    EXPERT_TUNING,
    MOTION_SINGLE_AXIS,
    DRIVE_PROGRAMMING,
    MAESTRO_SETUP_AND_MOTION
};
}
namespace TabToolBarConstants {
inline const QStringList PARAMETERS_GROUPS = { "maestrogroup", "drivegroup", "workspacegroup" };
inline const QStringList SYSTEM_CONFIGURATION_GROUPS = { "devicegroup" };
inline const QStringList UPLOAD_AND_DOWNLOAD_GROUPS = { "uploaddownloadmaestrogroup", "uploaddownloaddrivegroup" };
inline const QStringList QUICK_TUNING_GROUPS = { "quicktunerdevicegroup" };
inline const QStringList EXPERT_TUNING_GROUPS = { "experttunerdevicegroup" };
inline const QStringList TTBAR_QUICK_TUNING_STATE = { "drivequciktunerpage", "floatingtoolspage" };
inline const QStringList TTBAR_EXPERT_TUNING_STATE = { "driveexperttunerpage", "floatingtoolspage" };
inline const QStringList TTBAR_MOTION_SINGLE_AXIS_STATE = { "floatingtoolspage", "recordingpage", "viewdesignpage" };
inline const QStringList TTBAR_SYSTEM_CONFIGURATION_STATE = { "systemconfigurationpage", "uploadanddownloadpage", "floatingtoolspage" };
inline const QStringList TTBAR_ALL_PAGES = { "systemconfigurationpage", "drivequciktunerpage", "driveexperttunerpage",
                                            "toolsgrouppage", "uploadanddownloadpage", "floatingtoolspage", "recordingpage", "viewdesignpage"};
enum class TabToolVisibility {
    Show,
    Hide
};
}

namespace UIStateMachineConstants {
namespace WindowsListWidgetPagesTuningType {
inline const QEvent::Type QuickTuning = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type ExpertTuning = static_cast<QEvent::Type>(QEvent::registerEventType());
inline const QEvent::Type MotionSingleAxis = static_cast<QEvent::Type>(QEvent::registerEventType());
}
}

namespace PanelHeader {
inline const char * SYSTEM_CONFIGURATION = QT_TR_NOOP("Item Configuration");
inline const char * QUICK_TUNING = QT_TR_NOOP("Drive Quick Tuner");
inline const char * EXPERT_TUNING = QT_TR_NOOP("Drive Expert Tuner");
inline const char * MOTION_SINGLE_AXIS_PARAMETERS = QT_TR_NOOP("Single Axis Motion");
inline const char * MOTION_SINGLE_AXIS_RECORDER = QT_TR_NOOP("Recorder");
}


// L1 = Layer 1
namespace SettingsManagerConstants {
inline const QString COMPANY_NAME = "Altınay";
inline const QString APPLICATION_NAME = "Xmotion";

inline const QString L1_PAGE_GROUP = "PageParameters";
inline const QString LAST_QUICK_PAGE = "LastOpenedQuickTuningPage";
inline const QString LAST_EXPERT_PAGE = "LastOpenedExpertTuningPage";
inline const QString LAST_OPENED_WINDOW = "LastOpenedWindow";
inline const QString LAST_OPENED_SUB_MENU = "LastOpenedSubMenu";

inline const QString L1_WORKSPACE_GROUP = "Workspace";
inline const QString WORKSPACE = "WorkspaceName";
inline const QString L2_DEVIECS = "Devices";

inline const QString LOG_LEVEL = "LogLevel";
}

namespace MotorDriverTypes {
enum class MotorDriverType {
    PlatiniumDriveMotor,
    GoldDriveMotor
};
}

namespace ChartController {

inline const QVector<QColor> BaseColors = {
    QColor(255, 0, 0),     // Kırmızı
    QColor(0, 255, 0),     // Yeşil
    QColor(0, 0, 255),     // Mavi
    QColor(255, 255, 0),   // Sarı
    QColor(255, 0, 255),   // Macenta
    QColor(0, 255, 255),   // Camgöbeği
    QColor(128, 0, 0),     // Koyu Kırmızı
    QColor(0, 128, 0),     // Koyu Yeşil
    QColor(0, 0, 128),     // Koyu Mavi
    QColor(128, 128, 0),   // Zeytin
    QColor(128, 0, 128),   // Mor
    QColor(0, 128, 128),   // Turkuaz
    QColor(192, 192, 192), // Gümüş
    QColor(128, 128, 128), // Gri
    QColor(255, 165, 0),   // Turuncu
    QColor(255, 192, 203), // Pembe
    QColor(210, 105, 30),  // Çikolata
    QColor(173, 216, 230), // Açık Mavi
    QColor(0, 100, 0),     // Koyu Yeşil (alternatif)
    QColor(75, 0, 130)     // Çivit Mavisi
};
}
}

#endif // CONSTANTS_H
