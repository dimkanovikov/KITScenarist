#ifndef SCENARIOTEMPLATE_H
#define SCENARIOTEMPLATE_H

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
			SceneHeading,	//!< Время - место
			SceneCharacters,//!< Персонажи сцены
			Action,			//!< Описание действия
			Character,		//!< Имя героя
			Parenthetical,	//!< Ремарка
			Dialogue,			//!< Реплика героя
			Transition,		//!< Переход
			Note,			//!< Примечание
			TitleHeader,	//!< Заголовок титра
			Title,			//!< Текст титра
			NoprintableText,//!< Непечатный текст
			SceneGroupHeader,	//!< Заголовок группы сцен
			SceneGroupFooter,	//!< Окончание группы сцен
			FolderHeader,	//!< Заголовок папки
			FolderFooter,	//!< Окончание папки
			SceneDescription		//!< Описание элемента сценария
		};

		/**
		 * @brief Получить название типа
		 */
		static QString typeName(ScenarioBlockStyle::Type _type, bool _beautify = false);

		/**
		 * @brief Получить тип по названию
		 */
		static ScenarioBlockStyle::Type typeForName(const QString& _typeName, bool _beautify = false);

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
			PropertyIsCanModify,	//!< Редактируемый ли блок
			//
			PropertyIsReviewMark,	//!< Формат является редакторской правкой
			PropertyIsHighlight,	//!< Является ли правка аналогом выделения цветом из ворда
			PropertyIsDone,			//!< Правка помечена как выполненная
			PropertyComments,		//!< Список комментариев к правке
			PropertyCommentsAuthors,//!< Список авторов комментариев
			PropertyCommentsDates	//!< Список дат комментариев
		};

		/**
		 * @brief Виды межстрочных интервалов
		 */
		enum LineSpacing {
			SingleLineSpacing,
			OneAndHalfLineSpacing,
			DoubleLineSpacing,
			FixedLineSpacing
		};

	public:
		ScenarioBlockStyle() : m_type(Undefined), m_font(QFont("Courier New", 12)) {}

		/**
		 * @brief Получить тип блока
		 */
		static ScenarioBlockStyle::Type forBlock(const QTextBlock& _block);

		/**
		 * @brief Получить тип блока
		 */
		ScenarioBlockStyle::Type type() const { return m_type; }

		/**
		 * @brief Получить активность стиля блока
		 */
		bool isActive() const { return m_isActive; }

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
		 * @brief Отступ снизу, линий
		 */
		int bottomSpace() const { return m_bottomSpace; }

		/**
		 * @brief Отступ слева, мм
		 */
		qreal leftMargin() const { return m_leftMargin; }

		/**
		 * @brief Отступ сверху, мм
		 */
		qreal topMargin() const { return m_topMargin; }

		/**
		 * @brief Отступ справа, мм
		 */
		qreal rightMargin() const { return m_rightMargin; }

		/**
		 * @brief Отступ снизу, мм
		 */
		qreal bottomMargin() const { return m_bottomMargin; }

		/**
		 * @brief Заданы ли отступы в блоке в мм [true], или в линиях [false]
		 */
		bool hasVerticalSpacingInMM() const { return m_topMargin != 0 || m_bottomMargin != 0; }

		/**
		 * @brief Межстрочный интервал
		 */
		LineSpacing lineSpacing() const { return m_lineSpacing; }

		/**
		 * @brief Значение межстрочного интервала для FixedLineSpacing, мм
		 */
		qreal lineSpacingValue() const { return m_lineSpacingValue; }

		/**
		 * @brief Установить активность
		 */
		void setIsActive(bool _isActive);

		/**
		 * @brief Установить шрифт
		 */
		void setFont(const QFont& _font);

		/**
		 * @brief Установить выравнивание
		 */
		void setAlign(Qt::Alignment _align);

		/**
		 * @brief Установить отступ сверху, линий
		 */
		void setTopSpace(int _topSpace);

		/**
		 * @brief Установить отступ сверху, линий
		 */
		void setBottomSpace(int _bottomSpace);

		/**
		 * @brief Установить левый отступ, мм
		 */
		void setLeftMargin(qreal _leftMargin);

		/**
		 * @brief Установить верхний отступ, мм
		 */
		void setTopMargin(qreal _topMargin);

		/**
		 * @brief Установить правый отступ, мм
		 */
		void setRightMargin(qreal _rightMargin);

		/**
		 * @brief Установить нижний отступ, мм
		 */
		void setBottomMargin(qreal _bottomMargin);

		/**
		 * @brief Установить межстрочный интервал
		 */
		void setLineSpacing(LineSpacing _lineSpacing);

		/**
		 * @brief Значение межстрочного интервала для FixedLineSpacing, мм
		 */
		void setLineSpacingValue(qreal _value);

		/**
		 * @brief Настройки стиля отображения блока
		 */
		QTextBlockFormat blockFormat() const { return m_blockFormat; }

		/**
		 * @brief Установить цвет фона блока
		 */
		void setBackgroundColor(const QColor& _color);

		/**
		 * @brief Настройки шрифта блока
		 */
		QTextCharFormat charFormat() const { return m_charFormat; }

		/**
		 * @brief Установить цвет текста
		 */
		void setTextColor(const QColor& _color);

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
		void setPrefix(const QString& _prefix);

		/**
		 * @brief Постфикс стиля
		 */
		QString postfix() const;
		void setPostfix(const QString& _postfix);

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
		friend class ScenarioTemplate;

		/**
		 * @brief Обновить межстрочный интервал блока
		 */
		void updateLineHeight();

		/**
		 * @brief Обновить верхний отступ
		 */
		void updateTopMargin();

		/**
		 * @brief Обновить нижний отступ
		 */
		void updateBottomMargin();

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
		 * @brief Отступ снизу, линий
		 */
		int m_bottomSpace;

		/**
		 * @brief Отступ слева, мм
		 */
		qreal m_leftMargin;

		/**
		 * @brief Отступ сверху, мм
		 */
		qreal m_topMargin;

		/**
		 * @brief Отступ справа, мм
		 */
		qreal m_rightMargin;

		/**
		 * @brief Отступ снизу, мм
		 */
		qreal m_bottomMargin;

		/**
		 * @brief Межстрочный интервал
		 */
		LineSpacing m_lineSpacing;

		/**
		 * @brief Значение межстрочного интервала для FixedLineSpacing, мм
		 */
		qreal m_lineSpacingValue;

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
	 * @brief Класс шаблона сценария
	 */
	class ScenarioTemplate
	{
	public:
		ScenarioTemplate() {}

		/**
		 * @brief Сохранить шаблон в файл
		 */
		void saveToFile(const QString& _filePath) const;

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
		 * @brief Получить расположение нумерации
		 */
		Qt::Alignment numberingAlignment() const { return m_numberingAlignment; }

		/**
		 * @brief Получить стиль блока заданного типа
		 */
		ScenarioBlockStyle blockStyle(ScenarioBlockStyle::Type _forType) const;

		/**
		 * @brief Получить стиль заданного блока
		 */
		ScenarioBlockStyle blockStyle(const QTextBlock& _forBlock) const;

		/**
		 * @brief Установить наименование
		 */
		void setName(const QString& _name);

		/**
		 * @brief Установить описание
		 */
		void setDescription(const QString& _description);

		/**
		 * @brief Установить поля документа
		 */
		void setPageMargins(const QMarginsF& _pageMargins);

		/**
		 * @brief Установить расположение нумерации
		 */
		void setNumberingAlignment(Qt::Alignment _alignment);

		/**
		 * @brief Установить стиль блока
		 */
		void setBlockStyle(const ScenarioBlockStyle& _blockStyle);

		/**
		 * @brief Обновить цвета прорисовки блоков
		 */
		void updateBlocksColors();

	private:
		ScenarioTemplate(const QString& _fromFile);
		friend class ScenarioTemplateFacade;

		/**
		 * @brief Загрузить шаблон из файла
		 */
		void load(const QString& _fromFile);

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
		 * @brief Расположение нумерации
		 */
		Qt::Alignment m_numberingAlignment;

		/**
		 * @brief Стили блоков текста
		 */
		QMap<ScenarioBlockStyle::Type, ScenarioBlockStyle> m_blockStyles;
	};

	/**
	 * @brief Фасад доступа к шаблонам сценария
	 */
	class ScenarioTemplateFacade
	{
	public:
		/**
		 * @brief Получить список шаблонов
		 */
		static QStandardItemModel* templatesList();

		/**
		 * @brief Проверить существование шаблона с заданным именем
		 */
		static bool containsTemplate(const QString& _templateName);

		/**
		 * @brief Получить шаблон в соответствии с заданным именем
		 *
		 * Если имя не задано, возвращается стандартный шаблон
		 */
		static ScenarioTemplate getTemplate(const QString& _templateName = QString());

		/**
		 * @brief Сохранить стиль в библиотеке шаблонов
		 */
		/** @{ */
		static void saveTemplate(const ScenarioTemplate& _template);
		static bool saveTemplate(const QString& _templateFilePath);
		/** @} */

		/**
		 * @brief Удалить шаблон по заданному имены
		 */
		static void removeTemplate(const QString& _templateName);

		/**
		 * @brief Обновить цвета блоков текста для всех шаблонов
		 */
		static void updateTemplatesColors();

	private:
		ScenarioTemplateFacade();
		static ScenarioTemplateFacade* s_instance;
		static void init();

	private:
		/**
		 * @brief Шаблон по умолчанию
		 */
		ScenarioTemplate m_defaultTemplate;

		/**
		 * @brief Шаблоны сценариев
		 */
		QMap<QString, ScenarioTemplate> m_templates;

		/**
		 * @brief Модель шаблонов
		 */
		QStandardItemModel* m_templatesModel;
	};
}

#endif // SCENARIOTEMPLATE_H
