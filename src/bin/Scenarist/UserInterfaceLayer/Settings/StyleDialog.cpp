#include "StyleDialog.h"
#include "ui_StyleDialog.h"

#include <QFontDatabase>
#include <QStringListModel>

using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioStyle;
using UserInterface::StyleDialog;


StyleDialog::StyleDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StyleDialog)
{
	ui->setupUi(this);

	initView();
	initConnections();
}

StyleDialog::~StyleDialog()
{
	delete ui;
}

void StyleDialog::setScenarioStyle(const BusinessLogic::ScenarioStyle& _style, bool _isNew)
{
	//
	// Сохраним стиль
	//
	m_style = _style;

	//
	// Общие параметры стиля
	//
	if (_isNew) {
		ui->name->clear();
		ui->description->clear();
	} else {
		ui->name->setText(m_style.name());
		ui->description->setText(m_style.description());
	}
	ui->leftField->setValue(m_style.pageMargins().left());
	ui->topField->setValue(m_style.pageMargins().top());
	ui->rightField->setValue(m_style.pageMargins().right());
	ui->bottomField->setValue(m_style.pageMargins().bottom());
	int verticalAlignIndex = 0; // по умолчанию сверху
	if (m_style.numberingAlignment().testFlag(Qt::AlignBottom)) {
		verticalAlignIndex = 1;
	}
	ui->numberingVerticalAlignment->setCurrentIndex(verticalAlignIndex);
	int horizontalAlignIndex = 2; // по умолчанию справа
	if (m_style.numberingAlignment().testFlag(Qt::AlignLeft)) {
		horizontalAlignIndex = 0;
	} else if (m_style.numberingAlignment().testFlag(Qt::AlignCenter)) {
		horizontalAlignIndex = 1;
	}
	ui->numberingHorizontalAlignment->setCurrentIndex(horizontalAlignIndex);

	//
	// Очистим последний выбранный стиль блока
	//
	m_blockStyle = m_style.blockStyle(ScenarioBlockStyle::Undefined);
	//
	// ... и выберем первый из списка, для обновления интерфейса
	//
	ui->blockStyles->clearSelection();
	ui->blockStyles->setCurrentIndex(ui->blockStyles->model()->index(0, 0));
}

BusinessLogic::ScenarioStyle StyleDialog::scenarioStyle()
{
	//
	// Сохраним последний редактируемый стиль блока
	//
	aboutBlockStyleActivated(0);

	//
	// Сохраним основные параметры стиля
	//
	if (!ui->name->text().isEmpty()) {
		m_style.setName(ui->name->text());
	} else {
		m_style.setName(tr("Unnamed Style"));
	}
	m_style.setDescription(ui->description->text());
	m_style.setPageMargins(QMarginsF(ui->leftField->value(),
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
	m_style.setNumberingAlignment(numberingAlignment);

	//
	// Возвратим настроенный стиль
	//
	return m_style;
}

void StyleDialog::aboutBlockStyleActivated(QListWidgetItem* _item)
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
		}
		m_blockStyle.setAlign(align);

		m_blockStyle.setTopSpace(ui->topSpace->value());
		m_blockStyle.setLeftMargin(ui->leftIndent->value());
		m_blockStyle.setRightMargin(ui->rightIndent->value());

		//
		// Сохраним стиль блока в стиле сценария
		//
		m_style.setBlockStyle(m_blockStyle);

		//
		// Для группы сцен и папки, сохраняем так же и завершающий блок
		//
		if (m_blockStyle.isEmbeddableHeader()) {
			ScenarioBlockStyle footerBlockStyle = m_style.blockStyle(m_blockStyle.embeddableFooter());
			footerBlockStyle.setIsActive(m_blockStyle.isActive());
			footerBlockStyle.setFont(m_blockStyle.font());
			footerBlockStyle.setAlign(m_blockStyle.align());
			footerBlockStyle.setTopSpace(m_blockStyle.topSpace());
			footerBlockStyle.setLeftMargin(m_blockStyle.leftMargin());
			footerBlockStyle.setRightMargin(m_blockStyle.rightMargin());

			//
			// Сохраним стиль завершающего блока в стиле сценария
			//
			m_style.setBlockStyle(footerBlockStyle);
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
		ScenarioBlockStyle activatedBlockStyle = m_style.blockStyle(activatedType);

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
		}
		ui->alignment->setCurrentIndex(alignIndex);
		ui->topSpace->setValue(activatedBlockStyle.topSpace());
		ui->leftIndent->setValue(activatedBlockStyle.leftMargin());
		ui->rightIndent->setValue(activatedBlockStyle.rightMargin());

		//
		// Запомним стиль блока
		//
		m_blockStyle = activatedBlockStyle;
	}
	//
	// Если не задан элемент, сбросим последний сохранённый стиль блока
	//
	else {
		m_blockStyle = m_style.blockStyle(ScenarioBlockStyle::Undefined);
	}
}

void StyleDialog::initView()
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
}

void StyleDialog::initConnections()
{
	connect(ui->blockStyles, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(aboutBlockStyleActivated(QListWidgetItem*)));

	connect(ui->close, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->save, SIGNAL(clicked()), this, SLOT(accept()));
}
