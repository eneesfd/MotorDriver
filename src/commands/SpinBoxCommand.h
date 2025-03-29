#ifndef SPINBOXCOMMAND_H
#define SPINBOXCOMMAND_H

#include "ICommand.h"

namespace commands {
class SpinBoxCommand : public ICommand
{
public:
    SpinBoxCommand();
    void execute() override;
    void undo() override;
    void redo() override;
};
}

#endif // SPINBOXCOMMAND_H
