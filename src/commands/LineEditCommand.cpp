#include "LineEditCommand.h"

using namespace commands;

LineEditCommand::LineEditCommand(QLineEdit *lineEdit, const QString &newText) :
    m_lineEdit(lineEdit),
    m_newText(newText)
{}

void LineEditCommand::execute()
{
    m_lineEdit->setText(m_newText);
}

void LineEditCommand::undo()
{
    m_lineEdit->setText(m_oldText);
}

void LineEditCommand::redo()
{
    m_lineEdit->setText(m_newText);
}
