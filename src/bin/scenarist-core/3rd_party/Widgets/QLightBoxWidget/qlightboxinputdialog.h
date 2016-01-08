#ifndef QLIGHTBOXINPUTDIALOG_H
#define QLIGHTBOXINPUTDIALOG_H

#include "qlightboxdialog.h"

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;


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
		const QString& _text = QString::null);

	/**
	 * @brief Получить большой текст
	 */
	static QString getLongText(QWidget* _parent, const QString& _title, const QString& _label,
		const QString& _text = QString::null);

	/**
	 * @brief Выбор элемента из списка
	 */
	static QString getItem(QWidget* _parent, const QString& _title, const QStringList& _items,
		const QString& _selectedItem = QString::null);

private:
	explicit QLightBoxInputDialog(QWidget* _parent = 0, bool _isContentStretchable = false);

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
	QLineEdit* m_lineEdit;

	/**
	 * @brief Поле для ввода большого кол-ва текста
	 */
	QPlainTextEdit* m_textEdit;

	/**
	 * @brief Виджет для обработки списковых операций
	 */
	QListWidget* m_listWidget;

	/**
	 * @brief Кнопки диалога
	 */
	QDialogButtonBox* m_buttons;
};

#endif // QLIGHTBOXINPUTDIALOG_H
