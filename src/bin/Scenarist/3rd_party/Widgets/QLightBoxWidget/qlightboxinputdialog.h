#ifndef QLIGHTBOXINPUTDIALOG_H
#define QLIGHTBOXINPUTDIALOG_H

#include "qlightboxdialog.h"

class QDialogButtonBox;
class QLabel;
class QLineEdit;


/**
 * @brief Фабрика для простых диалогов ввода
 */
class QLightBoxInputDialog : public QLightBoxDialog
{
	Q_OBJECT

public:
	/**
	 * @brief Получить текст
	 */
	static QString getText(QWidget* _parent, const QString& _title, const QString& _label,
		const QString& _text = QString());

private:
	explicit QLightBoxInputDialog(QWidget* _parent = 0);

	/**
	 * @brief Настроить представление
	 */
	void initView();

	/**
	 * @brief Настроить соединения
	 */
	void initConnections();

	/**
	 * @brief Виджет на который установить фокус при отображении
	 */
	QWidget* focusedOnExec() const;

private:
	/**
	 * @brief Текстовая метка
	 */
	QLabel* m_label;

	/**
	 * @brief Поле для текстового ввода
	 */
	QLineEdit* m_textInput;

	/**
	 * @brief Кнопки диалога
	 */
	QDialogButtonBox* m_buttons;
};

#endif // QLIGHTBOXINPUTDIALOG_H
