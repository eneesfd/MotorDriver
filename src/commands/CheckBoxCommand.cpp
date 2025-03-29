#include "CheckBoxCommand.h"

using namespace commands;

CheckBoxCommand::CheckBoxCommand(QCheckBox* checkBox, bool newState) :
    m_checkBox(checkBox),
    m_newState(newState),
    m_oldState(checkBox->isChecked())
{

}

void CheckBoxCommand::execute()
{
    m_checkBox->setChecked(m_newState);
}

void CheckBoxCommand::undo()
{
    m_checkBox->setChecked(m_oldState);
}

void CheckBoxCommand::redo()
{
    m_checkBox->setChecked(m_newState);
}

void CheckBoxCommand::setOldState(bool oldState)
{
    m_oldState = oldState;
}
