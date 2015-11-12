#ifndef ACCEPTEBLELINEEDIT_H
#define ACCEPTEBLELINEEDIT_H

#include <QLineEdit>

class QToolButton;


/**
 * @brief Редактор строки, с возможностью фиксации/отмены введённого значения
 */
class AcceptebleLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	explicit AcceptebleLineEdit(QWidget* _parent = 0);

	/**
	 * @brief Установить текст, который будет считаться зафиксированным
	 */
	void setAcceptedText(const QString& _text);

	/**
	 * @brief Установить приставку вопроса о применении текста
	 */
	void setQuestionPrefix(const QString& _prefix);

signals:
	/**
	 * @brief Зафиксирован новый текст
	 */
	void textAccepted(const QString& _newText, const QString& _oldText);

	/**
	 * @brief Все изменения отменены
	 */
	void textRejected();

protected:
	/**
	 * @brief Переопределяется для позиционирования кнопок применить/отмена
	 */
	void resizeEvent(QResizeEvent*_event);

	/**
	 * @brief Переопределяется, для того чтобы повесить события на ENTER/ESC
	 */
	void keyPressEvent(QKeyEvent* _event);

	/**
	 * @brief Переопределяется, для вопроса о фиксации/отмене изменения текста
	 */
	void focusOutEvent(QFocusEvent* _event);

private slots:
	/**
	 * @brief Зафиксировать текст
	 */
	void acceptText();

	/**
	 * @brief Отменить изменения текста
	 */
	void rejectText();

	/**
	 * @brief Обновить видимость кнопок фиксации/отмены текста
	 */
	void updateButtonsVisibility();

private:
	/**
	 * @brief Кнопка применения текста
	 */
	QToolButton* m_accept;

	/**
	 * @brief Кнопка восстановления применённого текста
	 */
	QToolButton* m_reject;

	/**
	 * @brief Применённый текст
	 */
	QString m_acceptedText;

	/**
	 * @brief Приставка вопроса о применении, отмене изменений
	 */
	QString m_questionPrefix;
};

#endif // ACCEPTEBLELINEEDIT_H
