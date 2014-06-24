#include "ScenarioStyle.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Widgets/PagesTextEdit/PageMetrics.h>

#include <QDir>
#include <QFile>
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
	m_font.setPointSize(_blockAttributes.value("font_size").toInt());
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
	m_blockFormat.setTopMargin(QFontMetrics(m_font).lineSpacing() * m_topSpace);
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
	m_charFormat.setForeground(
				QColor(
					DataStorageLayer::StorageFacade::settingsStorage()->value(
						"scenario-editor/text-color",
						DataStorageLayer::SettingsStorage::ApplicationSettings)
					));

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

		case NoprintableText: {
			m_charFormat.setForeground(
						QColor(
							DataStorageLayer::StorageFacade::settingsStorage()->value(
								"scenario-editor/nonprintable-text-color",
								DataStorageLayer::SettingsStorage::ApplicationSettings)
							));
			break;
		}

		case FolderHeader:
		case FolderFooter: {
			m_blockFormat.setBackground(
						QColor(
							DataStorageLayer::StorageFacade::settingsStorage()->value(
								"scenario-editor/folder-background-color",
								DataStorageLayer::SettingsStorage::ApplicationSettings)
							));
			m_charFormat.setForeground(
						QColor(
							DataStorageLayer::StorageFacade::settingsStorage()->value(
								"scenario-editor/folder-text-color",
								DataStorageLayer::SettingsStorage::ApplicationSettings)
							));
			break;
		}

		default: {
			break;
		}
	}
}

// ********
// ScenarioStyle

ScenarioBlockStyle ScenarioStyle::blockStyle(ScenarioBlockStyle::Type _forType) const
{
	return m_blockStyles.value(_forType);
}

ScenarioStyle::ScenarioStyle(const QString& _from_file)
{
	load(_from_file);
}

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
			m_pageSizeId = PageMetrics::pageSizeId(styleAttributes.value("page_format").toString());
			m_pageMargins = ::marginsFromString(styleAttributes.value("page_margins").toString());

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
	return result;
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
	const QString defaultStylePath = stylesFolderPath + QDir::separator() + "default.xml";
	QFile defaultStyleFile(defaultStylePath);
	if (defaultStyleFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QFile defaultStyleRcFile(":/Styles/Styles/default.xml");
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
	const QString STYLE_FILE_SUFFIX = "xml";
	foreach (const QFileInfo& styleFile, stylesDir.entryInfoList(QDir::Files)) {
		if (styleFile.suffix() == STYLE_FILE_SUFFIX) {
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
