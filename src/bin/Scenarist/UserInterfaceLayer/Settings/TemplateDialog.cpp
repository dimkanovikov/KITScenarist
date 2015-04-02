#include "TemplateDialog.h"
#include "ui_TemplateDialog.h"

#include <QFontDatabase>
#include <QStringListModel>

using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTemplate;
using UserInterface::TemplateDialog;


TemplateDialog::TemplateDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TemplateDialog)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

TemplateDialog::~TemplateDialog()
{
	delete ui;
}

void TemplateDialog::setScenarioTemplate(const BusinessLogic::ScenarioTemplate& _template, bool _isNew)
{
	//
	// Сохраним шаблон
	//
	m_template = _template;

	//
	// Общие параметры шаблона
	//
	if (_isNew) {
		ui->name->clear();
		ui->description->clear();
	} else {
		ui->name->setText(m_template.name());
		ui->description->setText(m_template.description());
	}
	ui->leftField->setValue(m_template.pageMargins().left());
	ui->topField->setValue(m_template.pageMargins().top());
	ui->rightField->setValue(m_template.pageMargins().right());
	ui->bottomField->setValue(m_template.pageMargins().bottom());
	int verticalAlignIndex = 0; // по умолчанию сверху
	if (m_template.numberingAlignment().testFlag(Qt::AlignBottom)) {
		verticalAlignIndex = 1;
	}
	ui->numberingVerticalAlignment->setCurrentIndex(verticalAlignIndex);
	int horizontalAlignIndex = 2; // по умолчанию справа
	if (m_template.numberingAlignment().testFlag(Qt::AlignLeft)) {
		horizontalAlignIndex = 0;
	} else if (m_template.numberingAlignment().testFlag(Qt::AlignCenter)) {
		horizontalAlignIndex = 1;
	}
	ui->numberingHorizontalAlignment->setCurrentIndex(horizontalAlignIndex);

	//
	// Очистим последний выбранный стиль блока
	//
	m_blockStyle = m_template.blockStyle(ScenarioBlockStyle::Undefined);
	//
	// ... и выберем первый из списка, для обновления интерфейса
	//
	ui->blockStyles->clearSelection();
	ui->blockStyles->setCurrentIndex(ui->blockStyles->model()->index(0, 0));
}

BusinessLogic::ScenarioTemplate TemplateDialog::scenarioTemplate()
{
	//
	// Сохраним последний редактируемый стиль блока
	//
	aboutBlockStyleActivated(0);

	//
	// Сохраним основные параметры шаблона
	//
	if (!ui->name->text().isEmpty()) {
		m_template.setName(ui->name->text());
	} else {
		m_template.setName(tr("Unnamed Template"));
	}
	m_template.setDescription(ui->description->text());
	m_template.setPageMargins(QMarginsF(ui->leftField->value(),
									 ui->topField->value(),
									 ui->rightField->value(),
									 ui->bottomField->value()));
	Qt::Alignment numberingAlignment;
	switch (ui->numberingVerticalAlignment->currentIndex()) {
		default:
		case 0: numberingAlignment |= Qt::AlignTop; break;
		case 1: numberingAlignment |= Qt::AlignBottom; break;
	}
	switch (ui->numberingHorizontalAlignment->currentIndex()) {
		case 0: numberingAlignment |= Qt::AlignLeft; break;
		case 1: numberingAlignment |= Qt::AlignCenter; break;
		default:
		case 2: numberingAlignment |= Qt::AlignRight;
	}
	m_template.setNumberingAlignment(numberingAlignment);

	//
	// Возвратим настроенный шаблон
	//
	return m_template;
}

void TemplateDialog::aboutBlockStyleActivated(QListWidgetItem* _item)
{
	//
	// Сохраним предыдущие настройки стиля блока
	//
	if (m_blockStyle.type() != ScenarioBlockStyle::Undefined) {
		//
		// Обновим параметры стиля блока
		//
		m_blockStyle.setIsActive(ui->isActive->isChecked());

		QFont blockFont(ui->fontFamily->currentText(), ui->fontSize->value());
		blockFont.setBold(ui->bold->isChecked());
		blockFont.setItalic(ui->italic->isChecked());
		blockFont.setUnderline(ui->underline->isChecked());
		blockFont.setCapitalization(ui->uppercase->isChecked() ? QFont::AllUppercase : QFont::MixedCase);
		m_blockStyle.setFont(blockFont);

		Qt::Alignment align = Qt::AlignLeft;
		switch (ui->alignment->currentIndex()) {
			default:
			case 0: align = Qt::AlignLeft; break;
			case 1: align = Qt::AlignCenter; break;
			case 2: align = Qt::AlignRight; break;
			case 3: align = Qt::AlignJustify; break;
		}
		m_blockStyle.setAlign(align);

		m_blockStyle.setTopSpace(ui->topSpace->value());
		m_blockStyle.setBottomSpace(ui->bottomSpace->value());
		m_blockStyle.setLeftMargin(ui->leftIndent->value());
		m_blockStyle.setTopMargin(ui->topIndent->value());
		m_blockStyle.setRightMargin(ui->rightIndent->value());
		m_blockStyle.setBottomMargin(ui->bottomIndent->value());

		ScenarioBlockStyle::LineSpacing lineSpacing;
		switch (ui->lineSpacing->currentIndex()) {
			default:
			case 0: lineSpacing = ScenarioBlockStyle::SingleLineSpacing; break;
			case 1: lineSpacing = ScenarioBlockStyle::OneAndHalfLineSpacing; break;
			case 2: lineSpacing = ScenarioBlockStyle::DoubleLineSpacing; break;
			case 3: lineSpacing = ScenarioBlockStyle::FixedLineSpacing; break;
		}
		m_blockStyle.setLineSpacing(lineSpacing);
		m_blockStyle.setLineSpacingValue(ui->lineSpacingValue->value());
		if (ui->framingBrackets->isChecked()) {
			m_blockStyle.setPrefix("(");
			m_blockStyle.setPostfix(")");
		} else {
			m_blockStyle.setPrefix("");
			m_blockStyle.setPostfix("");
		}

		//
		// Сохраним стиль блока в шаблоне сценария
		//
		m_template.setBlockStyle(m_blockStyle);

		//
		// Для группы сцен и папки, сохраняем так же и завершающий блок
		//
		if (m_blockStyle.isEmbeddableHeader()) {
			ScenarioBlockStyle footerBlockStyle = m_template.blockStyle(m_blockStyle.embeddableFooter());
			footerBlockStyle.setIsActive(m_blockStyle.isActive());
			footerBlockStyle.setFont(m_blockStyle.font());
			footerBlockStyle.setAlign(m_blockStyle.align());
			footerBlockStyle.setTopSpace(m_blockStyle.topSpace());
			footerBlockStyle.setBottomSpace(m_blockStyle.bottomSpace());
			footerBlockStyle.setLeftMargin(m_blockStyle.leftMargin());
			footerBlockStyle.setTopMargin(m_blockStyle.topMargin());
			footerBlockStyle.setRightMargin(m_blockStyle.rightMargin());
			footerBlockStyle.setBottomMargin(m_blockStyle.bottomMargin());
			footerBlockStyle.setLineSpacing(m_blockStyle.lineSpacing());
			footerBlockStyle.setLineSpacingValue(m_blockStyle.lineSpacingValue());

			//
			// Сохраним стиль завершающего блока в шаблоне сценария
			//
			m_template.setBlockStyle(footerBlockStyle);
		}
	}

	//
	// Если задан элемент, загрузим его параметры
	//
	if (_item != 0) {
		//
		// Получим стиль активированного блока
		//
		ScenarioBlockStyle::Type activatedType = (ScenarioBlockStyle::Type)_item->type();
		ScenarioBlockStyle activatedBlockStyle = m_template.blockStyle(activatedType);

		//
		// Настроим представление
		//
		ui->isActive->setChecked(activatedBlockStyle.isActive());
		ui->fontFamily->setEditText(activatedBlockStyle.font().family());
		ui->fontSize->setValue(activatedBlockStyle.font().pointSize());
		ui->bold->setChecked(activatedBlockStyle.font().bold());
		ui->italic->setChecked(activatedBlockStyle.font().italic());
		ui->underline->setChecked(activatedBlockStyle.font().underline());
		ui->uppercase->setChecked(activatedBlockStyle.font().capitalization() == QFont::AllUppercase);
		int alignIndex = 0;
		switch (activatedBlockStyle.align()) {
			default:
			case Qt::AlignLeft: alignIndex = 0; break;
			case Qt::AlignCenter: alignIndex = 1; break;
			case Qt::AlignRight: alignIndex = 2; break;
			case Qt::AlignJustify: alignIndex = 3; break;
		}
		ui->alignment->setCurrentIndex(alignIndex);
		ui->topSpace->setValue(activatedBlockStyle.topSpace());
		ui->bottomSpace->setValue(activatedBlockStyle.bottomSpace());
		ui->leftIndent->setValue(activatedBlockStyle.leftMargin());
		ui->topIndent->setValue(activatedBlockStyle.topMargin());
		ui->rightIndent->setValue(activatedBlockStyle.rightMargin());
		ui->bottomIndent->setValue(activatedBlockStyle.bottomMargin());
		//
		// Настроим текущий тип вертикальных отступов блока
		//
		{
			if (activatedBlockStyle.hasVerticalSpacingInMM()) {
				ui->spacingType->setCurrentIndex(1);
			} else {
				ui->spacingType->setCurrentIndex(0);
			}
		}
		int lineSpacingIndex = 0;
		switch (activatedBlockStyle.lineSpacing()) {
			default:
			case ScenarioBlockStyle::SingleLineSpacing: lineSpacingIndex = 0; break;
			case ScenarioBlockStyle::OneAndHalfLineSpacing: lineSpacingIndex = 1; break;
			case ScenarioBlockStyle::DoubleLineSpacing: lineSpacingIndex = 2; break;
			case ScenarioBlockStyle::FixedLineSpacing: lineSpacingIndex = 3; break;
		}
		ui->lineSpacing->setCurrentIndex(lineSpacingIndex);
		ui->lineSpacingValue->setValue(activatedBlockStyle.lineSpacingValue());

		//
		// Настроим возможность обрамления скобками
		//
		ui->framingBrackets->setEnabled(activatedType == ScenarioBlockStyle::SceneCharacters);
		ui->framingBrackets->setChecked(activatedBlockStyle.hasDecoration());

		//
		// Запомним стиль блока
		//
		m_blockStyle = activatedBlockStyle;
	}
	//
	// Если не задан элемент, сбросим последний сохранённый стиль блока
	//
	else {
		m_blockStyle = m_template.blockStyle(ScenarioBlockStyle::Undefined);
	}
}

void TemplateDialog::aboutSpacingTypeChanged()
{
	const int LINE_SPACING_INDEX = 0;

	//
	// Настраиваем верхний и нижний отступы
	//
	// ... линиями
	//
	if (ui->spacingType->currentIndex() == LINE_SPACING_INDEX) {
		ui->topSpace->show();
		ui->bottomSpace->show();

		ui->topIndent->hide();
		ui->topIndent->setValue(0);
		ui->bottomIndent->hide();
		ui->bottomIndent->setValue(0);
	}
	//
	// ... в миллиметрах
	//
	else {
		ui->topIndent->show();
		ui->bottomIndent->show();

		ui->topSpace->hide();
		ui->topSpace->setValue(0);
		ui->bottomSpace->hide();
		ui->bottomSpace->setValue(0);
	}
}

void TemplateDialog::aboutLineSpacingChanged()
{
	//
	// Делаем активной возможность настройки точного межстрочного интервала, если необходимо
	//
	const int FIXED_LINE_SPACING_INDEX = 3;
	bool isEnabled = false;
	if (ui->lineSpacing->currentIndex() == FIXED_LINE_SPACING_INDEX) {
		isEnabled = true;
	}

	ui->lineSpacingValueLabel->setEnabled(isEnabled);
	ui->lineSpacingValue->setEnabled(isEnabled);
}

void TemplateDialog::initView()
{
	//
	// Формируем модель стилей блоков (она для всех стилей едина)
	//
	ui->blockStyles->addItem(new QListWidgetItem(tr("Time And Place"), 0, ScenarioBlockStyle::TimeAndPlace));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Scene Characters"), 0, ScenarioBlockStyle::SceneCharacters));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Action"), 0, ScenarioBlockStyle::Action));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Character"), 0, ScenarioBlockStyle::Character));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Parenthetical"), 0, ScenarioBlockStyle::Parenthetical));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Dialog"), 0, ScenarioBlockStyle::Dialog));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Transition"), 0, ScenarioBlockStyle::Transition));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Note"), 0, ScenarioBlockStyle::Note));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Title Header"), 0, ScenarioBlockStyle::TitleHeader));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Title"), 0, ScenarioBlockStyle::Title));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Noprintable Text"), 0, ScenarioBlockStyle::NoprintableText));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Scene Group"), 0, ScenarioBlockStyle::SceneGroupHeader));
	ui->blockStyles->addItem(new QListWidgetItem(tr("Folder"), 0, ScenarioBlockStyle::FolderHeader));

	//
	// Сформируем модель из списка шрифтов системы
	//
	QStringListModel* fontsModel = new QStringListModel(ui->fontFamily);
	fontsModel->setStringList(QFontDatabase().families());
	ui->fontFamily->setModel(fontsModel);

	//
	// Предварительная настройка типа вертикальных отступов
	//
	aboutSpacingTypeChanged();
}

void TemplateDialog::initConnections()
{
	connect(ui->blockStyles, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(aboutBlockStyleActivated(QListWidgetItem*)));
	connect(ui->spacingType, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutSpacingTypeChanged()));
	connect(ui->lineSpacing, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutLineSpacingChanged()));

	connect(ui->close, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->save, SIGNAL(clicked()), this, SLOT(accept()));
}
