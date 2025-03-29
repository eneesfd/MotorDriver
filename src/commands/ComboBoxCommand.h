#ifndef COMBOBOXCOMMAND_H
#define COMBOBOXCOMMAND_H

#include "ICommand.h"

namespace commands {
class ComboBoxCommand : public ICommand
{
public:
    ComboBoxCommand();
    void execute() override;
    void undo() override;
    void redo() override;
};
}

#endif // COMBOBOXCOMMAND_H
