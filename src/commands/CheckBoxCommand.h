#ifndef CHECKBOXCOMMAND_H
#define CHECKBOXCOMMAND_H

#include "ICommand.h"

#include <QCheckBox>

namespace commands {
class CheckBoxCommand : public ICommand
{
public:
    CheckBoxCommand(QCheckBox* checkBox, bool newState);
    void execute() override;
    void undo() override;
    void redo() override;
    void setOldState(bool oldState);
private:
    QCheckBox* m_checkBox;
    bool m_newState;
    bool m_oldState;
};
}

#endif // CHECKBOXCOMMAND_H
