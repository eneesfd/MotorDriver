#ifndef CURRENTIDENTIFICATIONCONTROLLER_H
#define CURRENTIDENTIFICATIONCONTROLLER_H

#include <BaseController/AbstractController.h>


class CurrentIdentificationController : public AbstractController
{
public:
    CurrentIdentificationController(QSharedPointer<CommandDispatcher> commandDispatcher_, std::shared_ptr<spdlog::logger> logger_, QObject* parent = nullptr);
    ~CurrentIdentificationController() override = default;
public slots:
    void identify();
};

#endif // CURRENTIDENTIFICATIONCONTROLLER_H
