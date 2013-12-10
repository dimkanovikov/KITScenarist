#ifndef SCENARIOTEXTBLOCKSTYLE_H
#define SCENARIOTEXTBLOCKSTYLE_H

#include <QTextBlockFormat>
#include <QTextCharFormat>

class ScenarioTextBlockStylePrivate;


/**
 * @brief Класс стиля блока текста сценария
 */
class ScenarioTextBlockStyle
{
public:
	/**
	 * @brief Виды блоков текста сценария
	 */
	enum Type {
		SceneHeader,	//!< Время - место
		Action,			//!< Описание действия
		Character,		//!< Имя героя
		Parenthetical,	//!< Ремарка
		Dialog,			//!< Реплика героя
		Transition,		//!< Переход
		NoteText,		//!< Примечание
		TitleHeader,	//!< Заголовок титра
		Title,			//!< Текст титра
		SceneGroupHeader, //!< Заголовок группы сцен
		SceneGroupFooter, //!< Окончание группы сцен
		SimpleText		//!< Простой текст
	};

	/**
	 * @brief Дополнительные свойства стилей текстовых блоков
	 */
	enum Property {
		PropertyType = QTextFormat::UserProperty + 1,
		PropertyPrefix,
		PropertyPostfix,
		PropertyIsFirstUppercase,
		PropertyIsCanModify
	};

public:
	ScenarioTextBlockStyle(ScenarioTextBlockStyle::Type _blockType);
	~ScenarioTextBlockStyle();

	/**
	 * @brief Вид блока
	 */
	ScenarioTextBlockStyle::Type blockType() const;

	/**
	 * @brief Настройки стиля отображения блока
	 */
	QTextBlockFormat blockFormat() const;

	/**
	 * @brief Настройки шрифта блока
	 */
	QTextCharFormat charFormat() const;

	/**
	 * @brief Первый символ заглавный
	 */
	bool isFirstUppercase() const;

	/**
	 * @brief Разрешено изменять текст блока
	 */
	bool isCanModify() const;

	/**
	 * @brief Имеет ли стиль обрамление
	 */
	bool hasDecoration() const;

	/**
	 * @brief Префикс стиля
	 */
	QString prefix() const;

	/**
	 * @brief Постфикс стиля
	 */
	QString postfix() const;

private:
	/**
	 * @brief Данные класса
	 */
	ScenarioTextBlockStylePrivate* m_pimpl;
};

#endif // SCENARIOTEXTBLOCKSTYLE_H
