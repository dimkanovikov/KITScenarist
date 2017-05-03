#include "AbstractKeyHandler.h"

#include "../ScenarioTextEdit.h"
#include <QKeyEvent>

using namespace KeyProcessingLayer;
using UserInterface::ScenarioTextEdit;


AbstractKeyHandler::AbstractKeyHandler(ScenarioTextEdit* _editor) :
    m_editor(_editor)
{
    Q_ASSERT(_editor);
}

AbstractKeyHandler::~AbstractKeyHandler()
{

}

void AbstractKeyHandler::handle(QKeyEvent* _event)
{
    prepareForHandle(_event);

    //
    // Получим необходимые переменные
    //
    // ... нажатая кнопка
    Qt::Key pressedKey = (Qt::Key)_event->key();
    // ... зажатые управляющие клавиши
    Qt::KeyboardModifiers pressedModifiers = _event->modifiers();

    //
    // Нажата клавиша Control
    // или Shift и событие не добавляет текст и не является выделением части текста
    // или Shift + Delete (вырезание)
    //
    static const QList<Qt::Key> keyboardNavigation({
        Qt::Key_Left, Qt::Key_Up, Qt::Key_Right, Qt::Key_Down , Qt::Key_PageUp, Qt::Key_PageDown
    });
    if (pressedModifiers.testFlag(Qt::ControlModifier)
        || (pressedModifiers.testFlag(Qt::ShiftModifier)
            && !keyboardNavigation.contains(pressedKey)
            && _event->text().isEmpty())
        || (pressedModifiers.testFlag(Qt::ShiftModifier)
            && pressedKey == Qt::Key_Delete)) {
        //
        // Ни чего не делаем, обработкой сочетаний клавиш занимаются ответственные за это классы
        //
    }

    //
    // Нажата клавиша "Enter"
    //
    else if (pressedKey == Qt::Key_Enter
             || pressedKey == Qt::Key_Return) {
        handleEnter(_event);
    }

    //
    // Нажата клавиша "Tab"
    //
    else if (pressedKey == Qt::Key_Tab) {
        handleTab(_event);
    }

    //
    // Нажата кнопка "Delete"
    //
    else if (pressedKey == Qt::Key_Delete) {
        handleDelete(_event);
    }

    //
    // Нажата кнопка "Backspace"
    //
    else if (pressedKey == Qt::Key_Backspace) {
        handleBackspace(_event);
    }

    //
    // Нажата кнопка "Escape"
    //
    else if (pressedKey == Qt::Key_Escape) {
        handleEscape(_event);
    }

    //
    // Нажата кнопка стрелочки "Вниз"
    //
    else if (pressedKey == Qt::Key_Down) {
        handleDown(_event);
    }

    //
    // Нажата кнопка стрелочки "Вверх"
    //
    else if (pressedKey == Qt::Key_Up) {
        handleUp(_event);
    }

    //
    // Нажата кнопка стрелочки "Страница вниз"
    //
    else if (pressedKey == Qt::Key_PageDown) {
        handlePageDown(_event);
    }

    //
    // Нажата кнопка стрелочки "Станица вверх"
    //
    else if (pressedKey == Qt::Key_PageUp) {
        handlePageUp(_event);
    }

    //
    // Нажата любая другая клавиша
    //
    else {
        handleOther(_event);
    }
}

ScenarioTextEdit* AbstractKeyHandler::editor() const
{
    return m_editor;
}
