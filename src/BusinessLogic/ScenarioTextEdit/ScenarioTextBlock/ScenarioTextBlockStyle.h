#ifndef SCENARIOTEXTBLOCKSTYLE_H
#define SCENARIOTEXTBLOCKSTYLE_H

#include <QTextBlockFormat>
#include <QTextCharFormat>

class ScenarioTextBlockStylePrivate;
class QTextBlock;


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
		Undefined,		//!< Неопределён
		TimeAndPlace,	//!< Время - место
		Action,			//!< Описание действия
		Character,		//!< Имя героя
		Parenthetical,	//!< Ремарка
		Dialog,			//!< Реплика героя
		Transition,		//!< Переход
		Note,			//!< Примечание
		TitleHeader,	//!< Заголовок титра
		Title,			//!< Текст титра
		NoprintableText,		//!< Простой текст
		SceneGroupHeader,	//!< Заголовок группы сцен
		SceneGroupFooter,	//!< Окончание группы сцен
		FolderHeader,	//!< Заголовок папки
		FolderFooter	//!< Окончание папки
	};

	/**
	 * @brief Дополнительные свойства стилей текстовых блоков
	 */
	enum Property {
		PropertyType = QTextFormat::UserProperty + 1, //!< Тип блока
		PropertyHeaderType,		//!< Тип блока заголовка
		PropertyHeader,			//!< Текст заголовка блока (а-ля ТИТР:)
		PropertyPrefix,			//!< Префикс блока
		PropertyPostfix,		//!< Постфикс блока
		PropertyIsFirstUppercase,	//!< Необходимо ли первый символ поднимать в верхний регистр
		PropertyIsCanModify,	//!< Редактируемый ли блок
		PropertyID				//!< Уникальный идентификатор блока
	};

public:
	ScenarioTextBlockStyle(ScenarioTextBlockStyle::Type _blockType);
	~ScenarioTextBlockStyle();

	/**
	 * @brief Получить тип блока
	 */
	static ScenarioTextBlockStyle::Type forBlock(const QTextBlock& _block);

	/**
	 * @brief Сменить стиль
	 */
	void setType(ScenarioTextBlockStyle::Type _type);

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

	/**
	 * @brief Имеет ли стиль заголовок
	 */
	bool hasHeader() const;

	/**
	 * @brief Вид заголовка
	 */
	ScenarioTextBlockStyle::Type headerType() const;

	/**
	 * @brief Заголовок стиля
	 */
	QString header() const;

	/**
	 * @brief Является ли стиль заголовком
	 */
	bool isHeader() const;

	/**
	 * @brief Является ли блок заголовком группы
	 */
	bool isEmbeddableHeader() const;

	/**
	 * @brief Блок закрывающий группу
	 */
	ScenarioTextBlockStyle::Type embeddableFooter() const;

private:
	/**
	 * @brief Данные класса
	 */
	ScenarioTextBlockStylePrivate* m_pimpl;
};

#endif // SCENARIOTEXTBLOCKSTYLE_H
