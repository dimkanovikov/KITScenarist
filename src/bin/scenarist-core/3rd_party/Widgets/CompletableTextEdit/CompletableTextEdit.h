#ifndef COMPLETABLETEXTEDIT_H
#define COMPLETABLETEXTEDIT_H

#include <3rd_party/Widgets/SpellCheckTextEdit/SpellCheckTextEdit.h>

class QCompleter;
class QAbstractItemModel;


/**
 * @brief Класс текстового редактора с подстановщиком для завершения текста
 */
class CompletableTextEdit : public SpellCheckTextEdit
{
	Q_OBJECT

public:
	explicit CompletableTextEdit(QWidget* _parent = 0);

    /**
     * @brief Установить необходимость использования подстановщика
     */
    void setUseCompleter(bool _use);

	/**
	 * @brief Получить подстановщика
	 */
	QCompleter* completer() const;

	/**
	 * @brief Открыт ли подстановщик
	 */
	bool isCompleterVisible() const;

	/**
	 * @brief Показать автодополнение текста
	 * @return Есть ли в модели для дополнения элементы с таким текстом
	 */
	bool complete(QAbstractItemModel* _model, const QString& _completionPrefix);

public slots:
	/**
	 * @brief Применить выбранный в подстановщике элемент
	 */
	void applyCompletion();
	void applyCompletion(const QString& _completion);

	/**
	 * @brief Закрыть подстановщика, если открыт
	 */
	void closeCompleter();

protected:
	/**
	 * @brief Можно ли показывать автодополнение
	 */
	virtual bool canComplete() const;

private:
    /**
     * @brief Использовать ли подстановщик
     */
    bool m_useCompleter;

	/**
	 * @brief Подстановщик для завершения текста
	 */
    QCompleter* m_completer;
};

#endif // COMPLETABLETEXTEDIT_H
