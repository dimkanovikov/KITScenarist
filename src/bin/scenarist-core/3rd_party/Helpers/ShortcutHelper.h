#ifndef SHORTCUTHELPER
#define SHORTCUTHELPER

#include <QKeySequence>


namespace ShortcutHelper
{
	/**
	 * @brief Сформировать платформозависимый шорткат
	 */
	static QString makeShortcut(const QString& _shortcut) {
		return QKeySequence(_shortcut).toString(QKeySequence::NativeText);
	}

	/**
	 * @brief Сформиовать платформозависимую подсказку
	 */
	static QString makeToolTip(const QString& _text, const QString& _shortcut) {
		return QString("%1 (%2)").arg(_text).arg(makeShortcut(_shortcut));
	}
}

#endif // SHORTCUTHELPER

