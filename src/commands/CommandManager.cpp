#include "CommandManager.h"

CommandManager::CommandManager() {}

void CommandManager::executeCommand(std::unique_ptr<commands::ICommand> cmd) {
    cmd->execute();
    m_undoStack.push(std::move(cmd));
    // Redo stack'ini temizleyebilirsiniz, çünkü yeni bir komut geldiğinde eski redo geçersiz kalır.
    while (!m_redoStack.empty()) m_redoStack.pop();
}

void CommandManager::undo() {
    if (!m_undoStack.empty()) {
        auto& cmd = m_undoStack.top();
        cmd->undo();
        m_redoStack.push(std::move(cmd));
        m_undoStack.pop();
    }
}

void CommandManager::redo() {
    if (!m_redoStack.empty()) {
        auto cmd = std::move(m_redoStack.top());
        cmd->execute();
        m_undoStack.push(std::move(cmd));
        m_redoStack.pop();
    }
}

void CommandManager::undoAll() {
    while(!m_undoStack.empty()) {
        undo();
    }
}

void CommandManager::applyAll() {
    // Uygulanmamış (henüz execute edilmemiş) komutları direkt olarak çalıştırma işlemi
    // veya en son durumları kalıcı hale getirme.
    // Bu uygulamanın mantığına bağlı olarak değişebilir.
}
