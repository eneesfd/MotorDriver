#ifndef RADIOBUTTONCOMMAND_H
#define RADIOBUTTONCOMMAND_H

#include "ICommand.h"

namespace commands {
class RadioButtonCommand : public ICommand
{
public:
    RadioButtonCommand();
    void execute() override;
    void undo() override;
    void redo() override;
};
}

#endif // RADIOBUTTONCOMMAND_H
