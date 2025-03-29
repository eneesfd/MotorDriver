#include "mainwindow.h"
#include <EventAggregator.h>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>

#define NAME_MAIN "Main"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MotorDriver_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QSharedPointer<BusinessStateMachineController> businessStateMachineController;
    try {
        businessStateMachineController.reset(new BusinessStateMachineController());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    QSharedPointer<UIStateMachineController> uiStateMachineController;
    try {
        uiStateMachineController.reset(new UIStateMachineController());
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    QSharedPointer<ApplicationController> applicationController;
    try {
        applicationController.reset(new ApplicationController(businessStateMachineController, uiStateMachineController));
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    MainWindow w(applicationController,uiStateMachineController);
    w.showMaximized();
    businessStateMachineController->start();
    uiStateMachineController->start();
    return a.exec();
}
