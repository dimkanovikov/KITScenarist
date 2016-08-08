#include "SimpleTextEditorWidget.h"
#include "SimpleTextEditor.h"

#include <3rd_party/Widgets/ColoredToolButton/ColoredToolButton.h>
#include <3rd_party/Widgets/FlatButton/FlatButton.h>
#include <3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h>

#include <QComboBox>
#include <QFontDatabase>
#include <QLabel>
#include <QSettings>
#include <QStringListModel>
#include <QVBoxLayout>


QVector<SimpleTextEditorWidget*> SimpleTextEditorWidget::s_editorsWidgets;

void SimpleTextEditorWidget::enableSpellCheck(bool _enable, SpellChecker::Language _language)
{
	//
	// Для каждого редактора
	//
	foreach (SimpleTextEditorWidget* editorWidget, s_editorsWidgets) {
		editorWidget->m_editor->setUseSpellChecker(_enable);
		editorWidget->m_editor->setSpellCheckLanguage(_language);
		editorWidget->m_editor->setHighlighterDocument(editorWidget->m_editor->document());
	}
}

SimpleTextEditorWidget::SimpleTextEditorWidget(QWidget *parent) :
	QWidget(parent),
	m_editor(new SimpleTextEditor(this)),
	m_editorWrapper(new ScalableWrapper(m_editor, this)),
	m_toolbar(new QWidget(this)),
	m_textFont(new QComboBox(this)),
	m_textFontSize(new QComboBox(this)),
	m_textBold(new FlatButton(this)),
	m_textItalic(new FlatButton(this)),
	m_textUnderline(new FlatButton(this)),
	m_textColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-color.png"), this)),
	m_textBackgroundColor(new ColoredToolButton(QIcon(":/Graphics/Icons/Review/font-bg-color.png"), this)),
	m_toolbarSpace(new QLabel(this)),
	m_isInTextFormatUpdate(false)
{
	initView();
	initConnections();
	initStyleSheet();

	s_editorsWidgets.append(this);
}

SimpleTextEditorWidget::~SimpleTextEditorWidget()
{
	s_editorsWidgets.removeOne(this);
}

void SimpleTextEditorWidget::setToolbarVisible(bool _visible)
{
	m_toolbar->setVisible(_visible);
}

void SimpleTextEditorWidget::setReadOnly(bool _readOnly)
{
	m_toolbar->setEnabled(!_readOnly);
	m_editor->setReadOnly(_readOnly);
}

void SimpleTextEditorWidget::setUsePageMode(bool _use)
{
	m_editor->setUsePageMode(_use);
	m_editor->setPageMargins(_use ? QMarginsF(20, 20, 20, 20) : QMarginsF(2, 2, 2, 2));
}

void SimpleTextEditorWidget::setPageSettings(QPageSize::PageSizeId _pageSize, const QMarginsF& _margins, Qt::Alignment _numberingAlign)
{
	m_editor->setPageFormat(_pageSize);
	m_editor->setPageMargins(_margins);
	m_editor->setPageNumbersAlignment(_numberingAlign);
}

QString SimpleTextEditorWidget::toHtml() const
{
	return m_editor->toHtml();
}

QString SimpleTextEditorWidget::toPlainText() const
{
	return m_editor->toPlainText();
}

void SimpleTextEditorWidget::setHtml(const QString& _html)
{
	m_editor->setHtml(_html);
}

void SimpleTextEditorWidget::clear()
{
	m_editor->clear();
}

SimpleTextEditor* SimpleTextEditorWidget::editor() const
{
	return m_editor;
}

void SimpleTextEditorWidget::initView()
{
	//
	// Обновить масштаб
	//
	QSettings settings;
	m_editorWrapper->setZoomRange(settings.value("simple-editor/zoom-range", 0).toDouble());

	m_textFont->setModel(new QStringListModel(QFontDatabase().families(), m_textFont));
	m_textFont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_textFont->setEditable(true);
	m_textFontSize->setModel(new QStringListModel({"8","9","10","11","12","14","18","24","30","36","48","60","72","96"}, m_textFontSize));
	m_textFontSize->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	m_textFontSize->setEditable(true);
	m_textBold->setCheckable(true);
	m_textBold->setShortcut(QKeySequence::Bold);
	m_textBold->setIcons(QIcon(":/Graphics/Icons/Editing/format-bold.png"));
    m_textBold->setToolTip(
            QString("%1 (%2)")
                .arg(tr("Make text bold"))
                .arg(m_textBold->shortcut().toString(QKeySequence::NativeText)));
	m_textItalic->setCheckable(true);
	m_textItalic->setShortcut(QKeySequence::Italic);
    m_textItalic->setIcons(QIcon(":/Graphics/Icons/Editing/format-italic.png"));
    m_textItalic->setToolTip(
            QString("%1 (%2)")
                .arg(tr("Make text italic"))
                .arg(m_textItalic->shortcut().toString(QKeySequence::NativeText)));
	m_textUnderline->setCheckable(true);
	m_textUnderline->setShortcut(QKeySequence::Underline);
    m_textUnderline->setIcons(QIcon(":/Graphics/Icons/Editing/format-underline.png"));
    m_textUnderline->setToolTip(
            QString("%1 (%2)")
                .arg(tr("Make text underline"))
                .arg(m_textUnderline->shortcut().toString(QKeySequence::NativeText)));
	m_textColor->setIconSize(QSize(20, 20));
	m_textColor->setColorsPane(ColoredToolButton::Google);
	m_textColor->setToolTip(tr("Change text color"));
	m_textBackgroundColor->setIconSize(QSize(20, 20));
	m_textBackgroundColor->setColorsPane(ColoredToolButton::Google);
	m_textBackgroundColor->setToolTip(tr("Change text background color"));
	m_toolbarSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	//
	// Настроим панель инструментов
	//
	QHBoxLayout* toolbarLayout = new QHBoxLayout;
	toolbarLayout->setSpacing(0);
	toolbarLayout->setContentsMargins(QMargins());
	toolbarLayout->addWidget(m_textFont);
	toolbarLayout->addWidget(m_textFontSize);
	toolbarLayout->addWidget(m_textBold);
	toolbarLayout->addWidget(m_textItalic);
	toolbarLayout->addWidget(m_textUnderline);
	toolbarLayout->addWidget(m_textColor);
	toolbarLayout->addWidget(m_textBackgroundColor);
	toolbarLayout->addWidget(m_toolbarSpace);
	m_toolbar->setLayout(toolbarLayout);
	//
	// Настроим осноную компоновку виджета
	//
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(QMargins());
	layout->addWidget(m_toolbar);
	layout->addWidget(m_editorWrapper);
	setLayout(layout);
}

void SimpleTextEditorWidget::initConnections()
{
	connect(m_editor, &SimpleTextEditor::textChanged, this, &SimpleTextEditorWidget::textChanged);

	connect(m_editorWrapper, &ScalableWrapper::zoomRangeChanged,
		[=] (const qreal _zoomRange) {
		//
		// Сохранить значение масштаба
		//
		QSettings settings;
		if (settings.value("simple-editor/zoom-range").toDouble() != _zoomRange) {
			settings.setValue("simple-editor/zoom-range", _zoomRange);
		}

		//
		// Обновить значение для всех простых редакторов
		//
		foreach (SimpleTextEditorWidget* editorWidget, s_editorsWidgets) {
			editorWidget->m_editorWrapper->setZoomRange(_zoomRange);
		}
	});

	//
	// Панель инструментов
	//
	connect(m_editor, &SimpleTextEditor::currentCharFormatChanged,
			[=] (const QTextCharFormat& _format) {
		m_isInTextFormatUpdate = true;
		const QFont font = _format.font();
		m_textFont->setCurrentText(font.family());
		m_textFontSize->setCurrentText(QString::number(font.pointSize()));
		m_textBold->setChecked(font.bold());
		m_textItalic->setChecked(font.italic());
		m_textUnderline->setChecked(font.underline());
		QColor textColor = palette().text().color();
		if (_format.hasProperty(QTextFormat::ForegroundBrush)) {
			textColor = _format.foreground().color();
		}
		m_textColor->setColor(textColor);
		QColor textBackgroundColor = palette().base().color();
		if (_format.hasProperty(QTextFormat::BackgroundBrush)) {
			textBackgroundColor = _format.background().color();
		}
		m_textBackgroundColor->setColor(textBackgroundColor);
		m_isInTextFormatUpdate = false;
	});
	//
	// ... шрифт
	//
	connect(m_textFont, &QComboBox::currentTextChanged, [=] {
		if (!m_isInTextFormatUpdate) {
			QFont font(m_textFont->currentText(), m_textFontSize->currentText().toInt());
			m_editor->setTextFont(font);
		}
	});
	//
	// ... размер шрифта
	//
	connect(m_textFontSize, &QComboBox::currentTextChanged, [=] {
		if (!m_isInTextFormatUpdate) {
			QFont font(m_textFont->currentText(), m_textFontSize->currentText().toInt());
			m_editor->setTextFont(font);
		}
	});
	//
	// ... начертания
	//
	connect(m_textBold, &FlatButton::toggled, [=] (bool _checked) {
		if (!m_isInTextFormatUpdate) {
			m_editor->setTextBold(_checked);
		}
	});
	connect(m_textItalic, &FlatButton::toggled, [=] (bool _checked) {
		if (!m_isInTextFormatUpdate) {
			m_editor->setTextItalic(_checked);
		}
	});
	connect(m_textUnderline, &FlatButton::toggled, [=] (bool _checked) {
		if (!m_isInTextFormatUpdate) {
			m_editor->setTextUnderline(_checked);
		}
	});
	//
	// ... цвет текста
	//
	connect(m_textColor, &ColoredToolButton::clicked, [=] (const QColor& _color) {
		if (!m_isInTextFormatUpdate) {
			m_editor->setTextColor(_color);
		}
	});
	//
	// ... цвет фона текста
	//
	connect(m_textBackgroundColor, &ColoredToolButton::clicked, [=] (const QColor& _color) {
		if (!m_isInTextFormatUpdate) {
			m_editor->setTextBackgroundColor(_color);
		}
	});
}

void SimpleTextEditorWidget::initStyleSheet()
{
	m_textFont->setProperty("inTopPanel", true);
	m_textFont->setProperty("topPanelTopBordered", true);
	m_textFont->setProperty("topPanelLeftBordered", true);
	m_textFont->setProperty("topPanelRightBordered", true);
	m_textFontSize->setProperty("inTopPanel", true);
	m_textFontSize->setProperty("topPanelTopBordered", true);
	m_textFontSize->setProperty("topPanelRightBordered", true);
	m_textBold->setProperty("inTopPanel", true);
	m_textItalic->setProperty("inTopPanel", true);
	m_textUnderline->setProperty("inTopPanel", true);
	m_textColor->setProperty("inTopPanel", true);
	m_textBackgroundColor->setProperty("inTopPanel", true);
	m_toolbarSpace->setProperty("inTopPanel", true);
	m_toolbarSpace->setProperty("topPanelTopBordered", true);
	m_toolbarSpace->setProperty("topPanelRightBordered", true);
}
