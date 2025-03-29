#ifndef SLIDERCOMMAND_H
#define SLIDERCOMMAND_H

#include "ICommand.h"

namespace commands {
class SliderCommand : public ICommand
{
public:
    SliderCommand();
    void execute() override;
    void undo() override;
    void redo() override;
};

}

#endif // SLIDERCOMMAND_H
