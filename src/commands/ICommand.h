#ifndef ICOMMAND_H
#define ICOMMAND_H

namespace commands  {
class ICommand {
public:
    virtual ~ICommand() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
};
}

#endif // ICOMMAND_H
