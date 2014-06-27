#ifndef SCENARIOSTYLE_H
#define SCENARIOSTYLE_H

#include "ScenarioStyle.h"

#include <QPageSize>
#include <QTextFormat>

class QStandardItemModel;
class QTextBlock;
class QXmlStreamAttributes;


namespace BusinessLogic
{
	/**
	 * @brief Класс стиля блока сценария
	 */
	class ScenarioBlockStyle
	{
	public:
		/**
		 * @brief Виды блоков текста сценария
		 */
		enum Type {
			Undefined,		//!< Неопределён
			TimeAndPlace,	//!< Время - место
			SceneCharacters,//!< Персонажи сцены
			Action,			//!< Описание действия
			Character,		//!< Имя героя
			Parenthetical,	//!< Ремарка
			Dialog,			//!< Реплика героя
			Transition,		//!< Переход
			Note,			//!< Примечание
			TitleHeader,	//!< Заголовок титра
			Title,			//!< Текст титра
			NoprintableText,//!< Непечатный текст
			SceneGroupHeader,	//!< Заголовок группы сцен
			SceneGroupFooter,	//!< Окончание группы сцен
			FolderHeader,	//!< Заголовок папки
			FolderFooter	//!< Окончание папки
		};

		/**
		 * @brief Получить название типа
		 */
		static QString typeName(ScenarioBlockStyle::Type _type);

		/**
		 * @brief Получить тип по названию
		 */
		static ScenarioBlockStyle::Type typeForName(const QString& _typeName);

		/**
		 * @brief Дополнительные свойства стилей текстовых блоков
		 */
		enum Property {
			PropertyType = QTextFormat::UserProperty + 100, //!< Тип блока
			PropertyHeaderType,		//!< Тип блока заголовка
			PropertyHeader,			//!< Текст заголовка блока (а-ля "ТИТР:")
			PropertyPrefix,			//!< Префикс блока
			PropertyPostfix,		//!< Постфикс блока
			PropertyIsFirstUppercase,	//!< Необходимо ли первый символ поднимать в верхний регистр
			PropertyIsCanModify		//!< Редактируемый ли блок
		};

	public:
		ScenarioBlockStyle() {}

		/**
		 * @brief Получить тип блока
		 */
		static ScenarioBlockStyle::Type forBlock(const QTextBlock& _block);

		/**
		 * @brief Получить тип блока
		 */
		ScenarioBlockStyle::Type type() const { return m_type; }

		/**
		 * @brief Получить шрифт блока
		 */
		QFont font() const { return m_font; }

		/**
		 * @brief Выравнивание блока
		 */
		Qt::Alignment align() const { return m_align; }

		/**
		 * @brief Отступ сверху, линий
		 */
		int topSpace() const { return m_topSpace; }

		/**
		 * @brief Отступ слева, мм
		 */
		qreal leftMargin() const { return m_leftMargin; }

		/**
		 * @brief Отступ справа, мм
		 */
		qreal rightMargin() const { return m_rightMargin; }

		/**
		 * @brief Настройки стиля отображения блока
		 */
		QTextBlockFormat blockFormat() const { return m_blockFormat; }

		/**
		 * @brief Настройки шрифта блока
		 */
		QTextCharFormat charFormat() const { return m_charFormat; }

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
		ScenarioBlockStyle::Type headerType() const;

		/**
		 * @brief Заголовок стиля
		 */
		QString header() const;

		/**
		 * @brief Является ли стиль заголовком
		 */
		bool isHeader() const;

		/**
		 * @brief Является ли блок частью группы
		 */
		bool isEmbeddable() const;

		/**
		 * @brief Является ли блок заголовком группы
		 */
		bool isEmbeddableHeader() const;

		/**
		 * @brief Блок закрывающий группу
		 */
		ScenarioBlockStyle::Type embeddableFooter() const;

	private:
		/**
		 * @brief Инициилизация возможна только в классе стиля сценария
		 */
		ScenarioBlockStyle(const QXmlStreamAttributes& _blockAttributes);
		friend class ScenarioStyle;

	private:
		/**
		 * @brief Тип блока
		 */
		Type m_type;

		/**
		 * @brief Активен ли блок
		 */
		bool m_isActive;

		/**
		 * @brief Шрифт блока
		 */
		QFont m_font;

		/**
		 * @brief Выравнивание блока
		 */
		Qt::Alignment m_align;

		/**
		 * @brief Отступ сверху, линий
		 */
		int m_topSpace;

		/**
		 * @brief Отступ слева, мм
		 */
		qreal m_leftMargin;

		/**
		 * @brief Отступ справа, мм
		 */
		qreal m_rightMargin;

		/**
		 * @brief Формат блока
		 */
		QTextBlockFormat m_blockFormat;

		/**
		 * @brief Формат текста
		 */
		QTextCharFormat m_charFormat;
	};

	/**
	 * @brief Класс стиля сценария
	 */
	class ScenarioStyle
	{
	public:
		ScenarioStyle() {}

		/**
		 * @brief Получить название
		 */
		QString name() const { return m_name; }

		/**
		 * @brief Получить описание
		 */
		QString description() const { return m_description; }

		/**
		 * @brief Получить размер страницы
		 */
		QPageSize::PageSizeId pageSizeId() const { return m_pageSizeId; }

		/**
		 * @brief Получить отступы страницы в миллиметрах
		 */
		QMarginsF pageMargins() const { return m_pageMargins; }

		/**
		 * @brief Получить стиль блока заданного типа
		 */
		ScenarioBlockStyle blockStyle(ScenarioBlockStyle::Type _forType) const;

	private:
		ScenarioStyle(const QString& _from_file);
		friend class ScenarioStyleFacade;

		/**
		 * @brief Загрузить стиль из файла
		 */
		void load(const QString& _from_file);

	private:
		/**
		 * @brief Название
		 */
		QString m_name;

		/**
		 * @brief Описание
		 */
		QString m_description;

		/**
		 * @brief Формат страницы
		 */
		QPageSize::PageSizeId m_pageSizeId;

		/**
		 * @brief Поля страницы в миллиметрах
		 */
		QMarginsF m_pageMargins;

		/**
		 * @brief Стили блоков текста
		 */
		QMap<ScenarioBlockStyle::Type, ScenarioBlockStyle> m_blockStyles;
	};

	/**
	 * @brief Фасад доступа к стилям сценария
	 */
	class ScenarioStyleFacade
	{
	public:
		/**
		 * @brief Получить список стилей
		 */
		static QStandardItemModel* stylesList();

		/**
		 * @brief Получить стиль в соответствии с заданным именем
		 *
		 * Если имя не задано, возвращается стандартный стиль
		 */
		static ScenarioStyle style(const QString& _styleName = QString());

	private:
		ScenarioStyleFacade();
		static ScenarioStyleFacade* s_instance;
		static void init();

	private:
		/**
		 * @brief Стиль по умолчанию
		 */
		ScenarioStyle m_defaultStyle;

		/**
		 * @brief Стили сценариев
		 */
		QMap<QString, ScenarioStyle> m_styles;

		/**
		 * @brief Модель стилей
		 */
		QStandardItemModel* m_stylesModel;
	};
}

#endif // SCENARIOSTYLE_H
