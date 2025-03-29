#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <memory>
#include <stack>
#include "ICommand.h"

class CommandManager
{
public:
    CommandManager();
    void executeCommand(std::unique_ptr<commands::ICommand> cmd);
    void undo();
    void redo();
    void undoAll();
    void applyAll();
private:
    std::stack<std::unique_ptr<commands::ICommand>> m_undoStack;
    std::stack<std::unique_ptr<commands::ICommand>> m_redoStack;
};

#endif // COMMANDMANAGER_H
