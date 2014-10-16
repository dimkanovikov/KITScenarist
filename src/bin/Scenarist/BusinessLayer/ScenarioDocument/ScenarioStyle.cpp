#include "ScenarioStyle.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QDir>
#include <QFile>
#include <QFontInfo>
#include <QFontMetrics>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QStringListModel>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QXmlStreamReader>

using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioStyle;
using BusinessLogic::ScenarioStyleFacade;

using BusinessLogic::ScenarioBlockStyle;


// ********
// ScenarioBlockStyle

namespace {
	/**
	 * @brief Получить карту типов и их текстовых отображений
	 */
	static QMap<ScenarioBlockStyle::Type, QString> typeNames() {
		static QMap<ScenarioBlockStyle::Type, QString> s_typeNames;
		if (s_typeNames.isEmpty()) {
			s_typeNames.insert(ScenarioBlockStyle::Undefined, "undefined");
			s_typeNames.insert(ScenarioBlockStyle::TimeAndPlace, "time_and_place");
			s_typeNames.insert(ScenarioBlockStyle::SceneCharacters, "scene_characters");
			s_typeNames.insert(ScenarioBlockStyle::Action, "action");
			s_typeNames.insert(ScenarioBlockStyle::Character, "character");
			s_typeNames.insert(ScenarioBlockStyle::Parenthetical, "parenthetical");
			s_typeNames.insert(ScenarioBlockStyle::Dialog, "dialog");
			s_typeNames.insert(ScenarioBlockStyle::Transition, "transition");
			s_typeNames.insert(ScenarioBlockStyle::Note, "note");
			s_typeNames.insert(ScenarioBlockStyle::TitleHeader, "title_header");
			s_typeNames.insert(ScenarioBlockStyle::Title, "title");
			s_typeNames.insert(ScenarioBlockStyle::NoprintableText, "noprintable_text");
			s_typeNames.insert(ScenarioBlockStyle::SceneGroupHeader, "scene_group_header");
			s_typeNames.insert(ScenarioBlockStyle::SceneGroupFooter, "scene_group_footer");
			s_typeNames.insert(ScenarioBlockStyle::FolderHeader, "folder_header");
			s_typeNames.insert(ScenarioBlockStyle::FolderFooter, "folder_footer");
		}
		return s_typeNames;
	}

	/**
	 * @brief Расширение файла стиля сценария
	 */
	const QString SCENARIO_STYLE_FILE_EXTENSION = "kitss";

	const qreal DEFINED_FONT_POINT_SIZEF = 12.0;

	/**
	 * @brief Предопределённые высоты шрифтов для размера 12pt
	 */
	static QMap<QString, qreal> definedFontLineHeight() {
		static QMap<QString, qreal> s_definedFontLineHeight;
		if (s_definedFontLineHeight.isEmpty()) {
			s_definedFontLineHeight.insert("Courier New", 18.09);
			s_definedFontLineHeight.insert("Times New Roman", 18.5);
			s_definedFontLineHeight.insert("Arial", 18.3);
			s_definedFontLineHeight.insert("CourierCyrillic", 13.1);
		}
		return s_definedFontLineHeight;
	}

	/**
	 * @brief Рассчитать высоту строки заданного шрифта
	 */
	static qreal fontLineHeight(const QFont& _font) {
		qreal realLineHeight = 0;

		//
		// Для некоторых шрифтов высоты предопределены
		//
		if (definedFontLineHeight().contains(_font.family())) {
			realLineHeight = definedFontLineHeight().value(_font.family()) * _font.pointSizeF() / DEFINED_FONT_POINT_SIZEF;
		} else {
			const QFontMetricsF fontMetrics(_font);
			realLineHeight = fontMetrics.lineSpacing();

			/*
			 * Пока оставим этот код до лучших времён
			 *

			//
			// Qt грязно округляет высоту линии считанную из файла шрифта,
			// поэтому восстанавливаем его при помощи следующей хитрости:
			// 1. определяем расстояние между строк для 100пт размера шрифта
			// 2. вычисляем расстояние мужду строк для заданного размера шрифта
			//
			QFont font100pt = _font;
			font100pt.setPointSizeF(100.0);
			QFontInfo font100ptInfo(font100pt);
			const QFontMetricsF font100ptMetrics(font100pt);
			const qreal lineHeightFont100pt = (font100ptMetrics.lineSpacing() + font100ptMetrics.height()) / 2;
			//
			// "- 0.1" - появилось, т.к. некоторые неточности не удаётся победить даже таким костылём
			//
			// ... работает для большинства протестированных мною шрифтов
			//
			realLineHeight = lineHeightFont100pt * _font.pointSizeF() / font100ptInfo.pointSizeF() - 0.1;
			*/
		}

		return realLineHeight;
	}
}

QString ScenarioBlockStyle::typeName(ScenarioBlockStyle::Type _type)
{
	return ::typeNames().value(_type);
}

ScenarioBlockStyle::Type ScenarioBlockStyle::typeForName(const QString& _typeName)
{
	return ::typeNames().key(_typeName);
}

ScenarioBlockStyle::Type ScenarioBlockStyle::forBlock(const QTextBlock& _block)
{
	ScenarioBlockStyle::Type blockType = ScenarioBlockStyle::Undefined;
	if (_block.blockFormat().hasProperty(ScenarioBlockStyle::PropertyType)) {
		blockType = (ScenarioBlockStyle::Type)_block.blockFormat().intProperty(ScenarioBlockStyle::PropertyType);
	}
	return blockType;
}

void ScenarioBlockStyle::setIsActive(bool _isActive)
{
	if (m_isActive != _isActive) {
		m_isActive = _isActive;
	}
}

void ScenarioBlockStyle::setFont(const QFont& _font)
{
	if (m_font != _font) {
		m_font = _font;

		m_charFormat.setFont(m_font);
		updateLineHeight();
	}
}

void ScenarioBlockStyle::setAlign(Qt::Alignment _align)
{
	if (m_align != _align) {
		m_align = _align;

		m_blockFormat.setAlignment(m_align);
		m_blockFormat.setTopMargin(QFontMetricsF(m_font).lineSpacing() * m_topSpace);
	}
}

void ScenarioBlockStyle::setTopSpace(int _topSpace)
{
	if (m_topSpace != _topSpace) {
		m_topSpace = _topSpace;

		m_blockFormat.setTopMargin(m_blockFormat.lineHeight() * m_topSpace);
	}
}

void ScenarioBlockStyle::setLeftMargin(qreal _leftMargin)
{
	if (m_leftMargin != _leftMargin) {
		m_leftMargin = _leftMargin;

		m_blockFormat.setLeftMargin(PageMetrics::mmToPx(m_leftMargin));
	}
}

void ScenarioBlockStyle::setRightMargin(qreal _rightMargin)
{
	if (m_rightMargin != _rightMargin) {
		m_rightMargin = _rightMargin;

		m_blockFormat.setRightMargin(PageMetrics::mmToPx(m_rightMargin));
	}
}

void ScenarioBlockStyle::setBackgroundColor(const QColor& _color)
{
	m_blockFormat.setBackground(_color);
}

void ScenarioBlockStyle::setTextColor(const QColor& _color)
{
	m_charFormat.setForeground(_color);
}

bool ScenarioBlockStyle::isFirstUppercase() const
{
	return m_charFormat.boolProperty(ScenarioBlockStyle::PropertyIsFirstUppercase);
}

bool ScenarioBlockStyle::isCanModify() const
{
	return m_charFormat.boolProperty(ScenarioBlockStyle::PropertyIsCanModify);
}

bool ScenarioBlockStyle::hasDecoration() const
{
	return !prefix().isEmpty() || !postfix().isEmpty();
}

QString ScenarioBlockStyle::prefix() const
{
	return m_charFormat.stringProperty(ScenarioBlockStyle::PropertyPrefix);
}

QString ScenarioBlockStyle::postfix() const
{
	return m_charFormat.stringProperty(ScenarioBlockStyle::PropertyPostfix);
}

bool ScenarioBlockStyle::hasHeader() const
{
	return !header().isEmpty();
}

ScenarioBlockStyle::Type ScenarioBlockStyle::headerType() const
{
	return (ScenarioBlockStyle::Type)m_blockFormat.intProperty(ScenarioBlockStyle::PropertyHeaderType);
}

QString ScenarioBlockStyle::header() const
{
	return m_blockFormat.stringProperty(ScenarioBlockStyle::PropertyHeader);
}

bool ScenarioBlockStyle::isHeader() const
{
	return m_type == ScenarioBlockStyle::TitleHeader;
}

bool ScenarioBlockStyle::isEmbeddable() const
{
	return  m_type == SceneGroupHeader
			|| m_type == SceneGroupFooter
			|| m_type == FolderHeader
			|| m_type == FolderFooter;
}

bool ScenarioBlockStyle::isEmbeddableHeader() const
{
	return  m_type == SceneGroupHeader
			|| m_type == FolderHeader;
}

ScenarioBlockStyle::Type ScenarioBlockStyle::embeddableFooter() const
{
	ScenarioBlockStyle::Type footer = Undefined;

	if (m_type == SceneGroupHeader) {
		footer = SceneGroupFooter;
	} else if (m_type == FolderHeader) {
		footer = FolderFooter;
	}

	return footer;
}

ScenarioBlockStyle::ScenarioBlockStyle(const QXmlStreamAttributes& _blockAttributes)
{
	//
	// Считываем параметры
	//
	// ... тип блока и его активность в стиле
	//
	m_type = typeForName(_blockAttributes.value ("id").toString ());
	m_isActive = _blockAttributes.value ("active").toString () == "true" ? true : false;
	//
	// ... настройки шрифта
	//
	m_font.setFamily(_blockAttributes.value ("font_family").toString());
	m_font.setPointSizeF(_blockAttributes.value("font_size").toDouble());
	//
	// ... начертание
	//
	m_font.setBold(_blockAttributes.value("bold").toString() == "true" ? true : false);
	m_font.setItalic(_blockAttributes.value("italic").toString() == "true" ? true : false);
	m_font.setUnderline(_blockAttributes.value("underline").toString() == "true" ? true : false);
	m_font.setCapitalization(_blockAttributes.value("uppercase").toString() == "true"
							 ? QFont::AllUppercase : QFont::MixedCase);

	//
	// ... расположение блока
	//
	QString alignment = _blockAttributes.value("alignment").toString();
	if (alignment == "left") {
		m_align = Qt::AlignLeft;
	} else if (alignment == "center") {
		m_align = Qt::AlignCenter;
	} else {
		m_align = Qt::AlignRight;
	}
	m_topSpace = _blockAttributes.value("top_space").toInt();
	m_leftMargin = _blockAttributes.value("left_margin").toDouble();
	m_rightMargin = _blockAttributes.value("right_margin").toDouble();

	//
	// Настроим форматы
	//
	// ... блока
	//
	m_blockFormat.setAlignment(m_align);
	updateLineHeight();
	m_blockFormat.setLeftMargin(PageMetrics::mmToPx(m_leftMargin));
	m_blockFormat.setRightMargin(PageMetrics::mmToPx(m_rightMargin));
	m_blockFormat.setBottomMargin(0);
	//
	// ... текста
	//
	m_charFormat.setFont(m_font);

	//
	// Запомним в стиле его настройки
	//
	m_blockFormat.setProperty(ScenarioBlockStyle::PropertyType, m_type);
	m_blockFormat.setProperty(ScenarioBlockStyle::PropertyHeaderType,
									 ScenarioBlockStyle::Undefined);
	m_charFormat.setProperty(ScenarioBlockStyle::PropertyIsFirstUppercase, true);
	m_charFormat.setProperty(ScenarioBlockStyle::PropertyIsCanModify, true);

	//
	// Настроим остальные характеристики
	//
	switch (m_type) {
		case Parenthetical: {
			m_charFormat.setProperty(ScenarioBlockStyle::PropertyIsFirstUppercase, false);
			m_charFormat.setProperty(ScenarioBlockStyle::PropertyPrefix, "(");
			m_charFormat.setProperty(ScenarioBlockStyle::PropertyPostfix, ")");
			break;
		}

		case TitleHeader: {
			m_charFormat.setProperty(ScenarioBlockStyle::PropertyIsCanModify, false);
			break;
		}

		case Title: {
			m_blockFormat.setProperty(ScenarioBlockStyle::PropertyHeaderType,
											 ScenarioBlockStyle::TitleHeader);
			m_blockFormat.setProperty(ScenarioBlockStyle::PropertyHeader,
											 QObject::tr("Title:", "ScenarioBlockStyle"));
			break;
		}

		case FolderFooter: {
			m_charFormat.setProperty(ScenarioBlockStyle::PropertyIsCanModify, false);
			break;
		}

		default: {
			break;
		}
	}
}

void ScenarioBlockStyle::updateLineHeight()
{
	const qreal lineHeight = ::fontLineHeight(m_font);
	m_blockFormat.setLineHeight(lineHeight, QTextBlockFormat::FixedHeight);
	m_blockFormat.setTopMargin(lineHeight * m_topSpace);
}

// ********
// ScenarioStyle

namespace {
	/**
	 * @brief Получить отступы из строки
	 */
	static QMarginsF marginsFromString(const QString& _margins) {
		QStringList margins = _margins.split(",");
		return QMarginsF(margins.value(0, 0).simplified().toDouble(),
						 margins.value(1, 0).simplified().toDouble(),
						 margins.value(2, 0).simplified().toDouble(),
						 margins.value(3, 0).simplified().toDouble());
	}

	/**
	 * @brief Получить строку из отступов
	 */
	static QString stringFromMargins(const QMarginsF& _margins) {
		return QString("%1,%2,%3,%4")
				.arg(_margins.left())
				.arg(_margins.top())
				.arg(_margins.right())
				.arg(_margins.bottom());
	}

	static Qt::Alignment alignmentFromString(const QString& _alignment) {
		Qt::Alignment result;
		//
		// Если нужно преобразовать несколько условий
		//
		if (_alignment.contains(",")) {
			foreach (const QString& align, _alignment.split(",")) {
				result |= ::alignmentFromString(align);
			}
		}
		//
		// Если нужно преобразовать одно выравнивание
		//
		else {
			if (_alignment == "top") result = Qt::AlignTop;
			else if (_alignment == "bottom") result = Qt::AlignBottom;
			else if (_alignment == "left") result = Qt::AlignLeft;
			else if (_alignment == "center") result = Qt::AlignCenter;
			else if (_alignment == "right") result = Qt::AlignRight;
		}
		return result;
	}

	/**
	 * @brief Преобразование разных типов в строку для записи в xml
	 */
	/** @{ */
	static QString toString(bool _value)  { return _value ? "true" : "false"; }
	static QString toString(int _value)   { return QString::number(_value); }
	static QString toString(qreal _value) { return QString::number(_value); }
	static QString toString(ScenarioBlockStyle::Type _value) {
		return ScenarioBlockStyle::typeName(_value);
	}
	static QString toString(Qt::Alignment _value) {
		QString result;

		if (_value.testFlag(Qt::AlignTop)) result.append("top");
		if (_value.testFlag(Qt::AlignBottom)) result.append("bottom");

		if (!result.isEmpty()) result.append(",");

		if (_value.testFlag(Qt::AlignLeft)) result.append("left");
		if (_value.testFlag(Qt::AlignCenter)) result.append("center");
		if (_value.testFlag(Qt::AlignRight)) result.append("right");

		return result;
	}

	/** @} */
}

void ScenarioStyle::saveToFile(const QString& _filePath) const
{
	QFile styleFile(_filePath);
	if (styleFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QXmlStreamWriter writer(&styleFile);
		writer.setAutoFormatting(true);
		writer.writeStartDocument();
		writer.writeStartElement("style");
		writer.writeAttribute("name", m_name);
		writer.writeAttribute("description", m_description);
		writer.writeAttribute("page_format", PageMetrics::stringFromPageSizeId(m_pageSizeId));
		writer.writeAttribute("page_margins", ::stringFromMargins(m_pageMargins));
		writer.writeAttribute("numbering_alignment", ::toString(m_numberingAlignment));
		foreach (const ScenarioBlockStyle& blockStyle, m_blockStyles.values()) {
			writer.writeStartElement("block");
			writer.writeAttribute("id", ::toString(blockStyle.type()));
			writer.writeAttribute("active", ::toString(blockStyle.isActive()));
			writer.writeAttribute("font_family", blockStyle.font().family());
			writer.writeAttribute("font_size", ::toString(blockStyle.font().pointSize()));
			writer.writeAttribute("bold", ::toString(blockStyle.font().bold()));
			writer.writeAttribute("italic", ::toString(blockStyle.font().italic()));
			writer.writeAttribute("underline", ::toString(blockStyle.font().underline()));
			writer.writeAttribute("uppercase", ::toString(blockStyle.font().capitalization()
														  == QFont::AllUppercase));
			writer.writeAttribute("alignment", ::toString(blockStyle.align()));
			writer.writeAttribute("top_space", ::toString(blockStyle.topSpace()));
			writer.writeAttribute("left_margin", ::toString(blockStyle.leftMargin()));
			writer.writeAttribute("right_margin", ::toString(blockStyle.rightMargin()));
			writer.writeEndElement(); // block
		}
		writer.writeEndElement(); // style
		writer.writeEndDocument();

		styleFile.close();
	}
}

ScenarioBlockStyle ScenarioStyle::blockStyle(ScenarioBlockStyle::Type _forType) const
{
	return m_blockStyles.value(_forType);
}

void ScenarioStyle::setName(const QString& _name)
{
	if (m_name != _name) {
		m_name = _name;
	}
}

void ScenarioStyle::setDescription(const QString& _description)
{
	if (m_description != _description) {
		m_description = _description;
	}
}

void ScenarioStyle::setPageMargins(const QMarginsF& _pageMargins)
{
	if (m_pageMargins != _pageMargins) {
		m_pageMargins = _pageMargins;
	}
}

void ScenarioStyle::setNumberingAlignment(Qt::Alignment _alignment)
{
	if (m_numberingAlignment != _alignment) {
		m_numberingAlignment = _alignment;
	}
}

void ScenarioStyle::setBlockStyle(const BusinessLogic::ScenarioBlockStyle& _blockStyle)
{
	m_blockStyles.insert(_blockStyle.type(), _blockStyle);
}

void ScenarioStyle::updateBlocksColors()
{
	//
	// Определим цвета
	//
	QColor mainTextColor =
			QColor(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/text-color",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	QColor noprintableTextColor =
			QColor(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/nonprintable-text-color",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	QColor folderTextColor =
			QColor(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/folder-text-color",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);
	QColor folderBackgroundColor =
			QColor(
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"scenario-editor/folder-background-color",
					DataStorageLayer::SettingsStorage::ApplicationSettings)
				);

	//
	// Обновим цвета блоков
	//
	foreach (ScenarioBlockStyle::Type blockStyleType, m_blockStyles.keys()) {
		ScenarioBlockStyle blockStyle = m_blockStyles.value(blockStyleType);
		switch (blockStyleType) {

			default: {
				blockStyle.setTextColor(mainTextColor);
				break;
			}

			case ScenarioBlockStyle::NoprintableText: {
				blockStyle.setTextColor(noprintableTextColor);
				break;
			}

			case ScenarioBlockStyle::FolderFooter:
			case ScenarioBlockStyle::FolderHeader: {
				blockStyle.setTextColor(folderTextColor);
				blockStyle.setBackgroundColor(folderBackgroundColor);
				break;
			}
		}
		m_blockStyles.insert(blockStyleType, blockStyle);
	}
}

ScenarioStyle::ScenarioStyle(const QString& _from_file)
{
	load(_from_file);
}

void ScenarioStyle::load(const QString& _from_file)
{
	QFile xmlData(_from_file);
	if (xmlData.open(QIODevice::ReadOnly)) {
		QXmlStreamReader reader(&xmlData);

		//
		// Считываем данные в соответствии с заданным форматом
		//
		if (reader.readNextStartElement() && (reader.name() == "style"))
		{
			//
			// Считываем атрибуты стиля
			//
			QXmlStreamAttributes styleAttributes = reader.attributes();
			m_name = styleAttributes.value("name").toString();
			m_description = styleAttributes.value("description").toString();
			m_pageSizeId = PageMetrics::pageSizeIdFromString(styleAttributes.value("page_format").toString());
			m_pageMargins = ::marginsFromString(styleAttributes.value("page_margins").toString());
			const QString numberingAlignment = styleAttributes.value("numbering_alignment").toString();
			if (!numberingAlignment.isEmpty()) {
				m_numberingAlignment = ::alignmentFromString(numberingAlignment);
			} else {
				m_numberingAlignment = Qt::AlignTop | Qt::AlignRight;
			}

			//
			// Считываем настройки оформления блоков текста
			//
			while (reader.readNextStartElement() && (reader.name() == "block"))
			{
				ScenarioBlockStyle block(reader.attributes());
				m_blockStyles.insert(block.type(), block);

				//
				// Если ещё не находимся в конце элемента, то остальное пропускаем
				//
				if (!reader.isEndElement())
					reader.skipCurrentElement();
			}
		}
	}
}

// ********
// ScenarioStyleFacade

QStandardItemModel* ScenarioStyleFacade::stylesList()
{
	init();

	return s_instance->m_stylesModel;
}

bool ScenarioStyleFacade::containsStyle(const QString& _styleName)
{
	init();

	return s_instance->m_styles.contains(_styleName);
}

ScenarioStyle ScenarioStyleFacade::style(const QString& _styleName)
{
	init();

	const QString currentStyle =
			DataStorageLayer::StorageFacade::settingsStorage()->value(
				"scenario-editor/current-style",
				DataStorageLayer::SettingsStorage::ApplicationSettings);

	ScenarioStyle result;
	if (_styleName.isEmpty()) {
		if (!currentStyle.isEmpty()
			&& s_instance->m_styles.contains(currentStyle)) {
			result = s_instance->m_styles.value(currentStyle);
		} else {
			result = s_instance->m_defaultStyle;
		}
	} else {
		result = s_instance->m_styles.value(_styleName);
	}

	result.updateBlocksColors();

	return result;
}

void ScenarioStyleFacade::saveStyle(const BusinessLogic::ScenarioStyle& _style)
{
	init();

	//
	// Если такого стиля ещё не было раньше, то добавляем строку в модель стилей
	//
	if (!containsStyle(_style.name())) {
		QStandardItem* stylesRootItem = s_instance->m_stylesModel->invisibleRootItem();
		QList<QStandardItem*> styleRow;
		styleRow << new QStandardItem(_style.name());
		styleRow << new QStandardItem(_style.description());
		stylesRootItem->appendRow(styleRow);
	}
	//
	// Добавляем/обновляем стиль в библиотеке
	//
	s_instance->m_styles.insert(_style.name(), _style);


	//
	// Настроим путь к папке со стилями
	//
	const QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	const QString stylesFolderPath = appDataFolderPath + QDir::separator() + "Styles";
	const QString styleFilePath =
			stylesFolderPath + QDir::separator()
			+ _style.name() + "." + SCENARIO_STYLE_FILE_EXTENSION;
	//
	// Сохраняем стиль в файл
	//
	_style.saveToFile(styleFilePath);
}

bool ScenarioStyleFacade::saveStyle(const QString& _styleFilePath)
{
	init();

	//
	// Загружаем стиль из файла
	//
	ScenarioStyle newStyle(_styleFilePath);

	//
	// Если загрузка произошла успешно, то добавляем его в библиотеку
	//
	bool styleSaved = false;
	if (!newStyle.name().isEmpty()) {
		saveStyle(newStyle);
		styleSaved = true;
	}

	return styleSaved;
}

void ScenarioStyleFacade::removeStyle(const QString& _styleName)
{
	init();

	//
	// Удалим стиль из библиотеки
	//
	s_instance->m_styles.remove(_styleName);
	foreach (QStandardItem* styleItem, s_instance->m_stylesModel->findItems(_styleName)) {
		s_instance->m_stylesModel->removeRow(styleItem->row());
	}

	//
	// Настроим путь к папке со стилями
	//
	const QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	const QString stylesFolderPath = appDataFolderPath + QDir::separator() + "Styles";
	const QString styleFilePath =
			stylesFolderPath + QDir::separator()
			+ _styleName + "." + SCENARIO_STYLE_FILE_EXTENSION;
	//
	// Удалим файл со стилем
	//
	QFile::remove(styleFilePath);
}

ScenarioStyleFacade::ScenarioStyleFacade()
{
	//
	// Настроим путь к папке со стилями
	//
	const QString appDataFolderPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	const QString stylesFolderPath = appDataFolderPath + QDir::separator() + "Styles";
	//
	// ... создаём папку для пользовательских файлов
	//
	QDir rootFolder = QDir::root();
	rootFolder.mkpath(stylesFolderPath);

	//
	// Сохраним стиль по умолчанию, если необходимо
	//
	const QString defaultStylePath =
			stylesFolderPath + QDir::separator()
			+ "default." + SCENARIO_STYLE_FILE_EXTENSION;
	QFile defaultStyleFile(defaultStylePath);
	if (defaultStyleFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QFile defaultStyleRcFile(":/Styles/Styles/default." + SCENARIO_STYLE_FILE_EXTENSION);
		if (defaultStyleRcFile.open(QIODevice::ReadOnly)) {
			defaultStyleFile.write(defaultStyleRcFile.readAll());
			defaultStyleRcFile.close();
		}
		defaultStyleFile.close();
	}

	//
	// Загрузить стили
	//
	QDir stylesDir(stylesFolderPath);
	foreach (const QFileInfo& styleFile, stylesDir.entryInfoList(QDir::Files)) {
		if (styleFile.suffix() == SCENARIO_STYLE_FILE_EXTENSION) {
			ScenarioStyle style(styleFile.absoluteFilePath());
			m_styles.insert(style.name(), style);
		}
	}
	//
	// ... стиль по умолчанию
	//
	m_defaultStyle = ScenarioStyle(defaultStylePath);

	//
	// Настроим модель стилей
	//
	m_stylesModel = new QStandardItemModel;
	QStandardItem* rootItem = m_stylesModel->invisibleRootItem();
	foreach (const ScenarioStyle& style, m_styles.values()) {
		QList<QStandardItem*> row;
		row << new QStandardItem(style.name());
		row << new QStandardItem(style.description());

		rootItem->appendRow(row);
	}
}

void ScenarioStyleFacade::init()
{
	//
	// Если необходимо создаём одиночку
	//
	if (s_instance == 0) {
		s_instance = new ScenarioStyleFacade;
	}
}

ScenarioStyleFacade* ScenarioStyleFacade::s_instance = 0;
