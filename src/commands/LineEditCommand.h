#ifndef LINEEDITCOMMAND_H
#define LINEEDITCOMMAND_H

#include <QLineEdit>
#include "ICommand.h"

namespace commands {
class LineEditCommand : public ICommand
{
public:
    LineEditCommand(QLineEdit* lineEdit, const QString& newText);
    void execute() override;
    void undo() override;
    void redo() override;
    void setOldText(const QString &oldText) {m_oldText = oldText;}
private:
    QLineEdit* m_lineEdit;
    QString m_newText;
    QString m_oldText;
};
}
#endif // LINEEDITCOMMAND_H
