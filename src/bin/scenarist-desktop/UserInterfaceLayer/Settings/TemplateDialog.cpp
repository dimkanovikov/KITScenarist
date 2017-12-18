#include "TemplateDialog.h"
#include "ui_TemplateDialog.h"

#include <QFontDatabase>
#include <QPushButton>
#include <QStringListModel>

using BusinessLogic::ScenarioBlockStyle;
using BusinessLogic::ScenarioTemplate;
using UserInterface::TemplateDialog;

namespace {
    /**
     * @brief Сформировать элемент списка для заданного типа блока
     */
    static QListWidgetItem* makeListWidgetItem(ScenarioBlockStyle::Type _forType) {
        static const bool BEAUTIFY_NAME = true;
        return new QListWidgetItem(ScenarioBlockStyle::typeName(_forType, BEAUTIFY_NAME), 0, _forType);
    }
}


TemplateDialog::TemplateDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::TemplateDialog)
{
    m_ui->setupUi(this);

    connect(this, &TemplateDialog::showed, [=] { m_ui->blockStyles->setCurrentRow(0); });

    initStyleSheet();
}

TemplateDialog::~TemplateDialog()
{
    delete m_ui;
}

void TemplateDialog::setScenarioTemplate(const BusinessLogic::ScenarioTemplate& _template)
{
    //
    // Сохраним шаблон
    //
    m_template = _template;

    //
    // Общие параметры шаблона
    //
    m_ui->name->setText(m_template.name());
    m_ui->description->setText(m_template.description());

    if (m_template.pageSizeId() == QPageSize::A4) {
        m_ui->pageFormatA4->setChecked(true);
    } else {
        m_ui->pageFormatLetter->setChecked(true);
    }
    m_ui->leftField->setValue(m_template.pageMargins().left());
    m_ui->topField->setValue(m_template.pageMargins().top());
    m_ui->rightField->setValue(m_template.pageMargins().right());
    m_ui->bottomField->setValue(m_template.pageMargins().bottom());
    int verticalAlignIndex = 0; // по умолчанию сверху
    if (m_template.numberingAlignment().testFlag(Qt::AlignBottom)) {
        verticalAlignIndex = 1;
    }
    m_ui->numberingVerticalAlignment->setCurrentIndex(verticalAlignIndex);
    int horizontalAlignIndex = 2; // по умолчанию справа
    if (m_template.numberingAlignment().testFlag(Qt::AlignLeft)) {
        horizontalAlignIndex = 0;
    } else if (m_template.numberingAlignment().testFlag(Qt::AlignCenter)) {
        horizontalAlignIndex = 1;
    }
    m_ui->numberingHorizontalAlignment->setCurrentIndex(horizontalAlignIndex);

    //
    // Очистим последний выбранный стиль блока
    //
    m_blockStyle = m_template.blockStyle(ScenarioBlockStyle::Undefined);

    //
    // Установим доступный режим работы с шаблоном
    //
    setReadOnly(m_template.isDefault());
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
    if (!m_ui->name->text().isEmpty()) {
        m_template.setName(m_ui->name->text());
    } else {
        m_template.setName(tr("Unnamed Template"));
    }
    m_template.setDescription(m_ui->description->text());
    m_template.setPageSizeId(m_ui->pageFormatA4->isChecked() ? QPageSize::A4 : QPageSize::Letter);
    m_template.setPageMargins(QMarginsF(m_ui->leftField->value(),
                                     m_ui->topField->value(),
                                     m_ui->rightField->value(),
                                     m_ui->bottomField->value()));
    Qt::Alignment numberingAlignment;
    switch (m_ui->numberingVerticalAlignment->currentIndex()) {
        default:
        case 0: numberingAlignment |= Qt::AlignTop; break;
        case 1: numberingAlignment |= Qt::AlignBottom; break;
    }
    switch (m_ui->numberingHorizontalAlignment->currentIndex()) {
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
        m_blockStyle.setIsActive(m_ui->isActive->isChecked());

        QFont blockFont(m_ui->fontFamily->currentText(), m_ui->fontSize->value());
        blockFont.setBold(m_ui->bold->isChecked());
        blockFont.setItalic(m_ui->italic->isChecked());
        blockFont.setUnderline(m_ui->underline->isChecked());
        blockFont.setCapitalization(m_ui->uppercase->isChecked() ? QFont::AllUppercase : QFont::MixedCase);
        m_blockStyle.setFont(blockFont);

        Qt::Alignment align = Qt::AlignLeft;
        switch (m_ui->alignment->currentIndex()) {
            default:
            case 0: align = Qt::AlignLeft; break;
            case 1: align = Qt::AlignCenter; break;
            case 2: align = Qt::AlignRight; break;
            case 3: align = Qt::AlignJustify; break;
        }
        m_blockStyle.setAlign(align);

        m_blockStyle.setTopSpace(m_ui->topSpace->value());
        m_blockStyle.setBottomSpace(m_ui->bottomSpace->value());
        m_blockStyle.setLeftMargin(m_ui->leftIndent->value());
        m_blockStyle.setTopMargin(m_ui->topIndent->value());
        m_blockStyle.setRightMargin(m_ui->rightIndent->value());
        m_blockStyle.setBottomMargin(m_ui->bottomIndent->value());

        ScenarioBlockStyle::LineSpacing lineSpacing;
        switch (m_ui->lineSpacing->currentIndex()) {
            default:
            case 0: lineSpacing = ScenarioBlockStyle::SingleLineSpacing; break;
            case 1: lineSpacing = ScenarioBlockStyle::OneAndHalfLineSpacing; break;
            case 2: lineSpacing = ScenarioBlockStyle::DoubleLineSpacing; break;
            case 3: lineSpacing = ScenarioBlockStyle::FixedLineSpacing; break;
        }
        m_blockStyle.setLineSpacing(lineSpacing);
        m_blockStyle.setLineSpacingValue(m_ui->lineSpacingValue->value());
        if (m_ui->framingBrackets->isChecked()) {
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
        m_ui->isActive->setChecked(activatedBlockStyle.isActive());
        m_ui->fontFamily->setEditText(activatedBlockStyle.font().family());
        m_ui->fontSize->setValue(activatedBlockStyle.font().pointSize());
        m_ui->bold->setChecked(activatedBlockStyle.font().bold());
        m_ui->italic->setChecked(activatedBlockStyle.font().italic());
        m_ui->underline->setChecked(activatedBlockStyle.font().underline());
        m_ui->uppercase->setChecked(activatedBlockStyle.font().capitalization() == QFont::AllUppercase);
        int alignIndex = 0;
        switch (activatedBlockStyle.align()) {
            default:
            case Qt::AlignLeft: alignIndex = 0; break;
            case Qt::AlignCenter: alignIndex = 1; break;
            case Qt::AlignRight: alignIndex = 2; break;
            case Qt::AlignJustify: alignIndex = 3; break;
        }
        m_ui->alignment->setCurrentIndex(alignIndex);
        m_ui->topSpace->setValue(activatedBlockStyle.topSpace());
        m_ui->bottomSpace->setValue(activatedBlockStyle.bottomSpace());
        m_ui->leftIndent->setValue(activatedBlockStyle.leftMargin());
        m_ui->topIndent->setValue(activatedBlockStyle.topMargin());
        m_ui->rightIndent->setValue(activatedBlockStyle.rightMargin());
        m_ui->bottomIndent->setValue(activatedBlockStyle.bottomMargin());
        //
        // Настроим текущий тип вертикальных отступов блока
        //
        {
            if (activatedBlockStyle.hasVerticalSpacingInMM()) {
                m_ui->spacingType->setCurrentIndex(1);
            } else {
                m_ui->spacingType->setCurrentIndex(0);
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
        m_ui->lineSpacing->setCurrentIndex(lineSpacingIndex);
        m_ui->lineSpacingValue->setValue(activatedBlockStyle.lineSpacingValue());

        //
        // Настроим возможность обрамления скобками
        //
        m_ui->framingBrackets->setEnabled(activatedType == ScenarioBlockStyle::SceneCharacters);
        m_ui->framingBrackets->setChecked(activatedBlockStyle.hasDecoration());

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
    if (m_ui->spacingType->currentIndex() == LINE_SPACING_INDEX) {
        m_ui->topSpace->show();
        m_ui->bottomSpace->show();

        m_ui->topIndent->hide();
        m_ui->topIndent->setValue(0);
        m_ui->bottomIndent->hide();
        m_ui->bottomIndent->setValue(0);
    }
    //
    // ... в миллиметрах
    //
    else {
        m_ui->topIndent->show();
        m_ui->bottomIndent->show();

        m_ui->topSpace->hide();
        m_ui->topSpace->setValue(0);
        m_ui->bottomSpace->hide();
        m_ui->bottomSpace->setValue(0);
    }
}

void TemplateDialog::aboutLineSpacingChanged()
{
    //
    // Делаем активной возможность настройки точного межстрочного интервала, если необходимо
    //
    const int FIXED_LINE_SPACING_INDEX = 3;
    bool isEnabled = false;
    if (m_ui->lineSpacing->currentIndex() == FIXED_LINE_SPACING_INDEX) {
        isEnabled = true;
    }

    m_ui->lineSpacingValueLabel->setEnabled(isEnabled);
    m_ui->lineSpacingValue->setEnabled(isEnabled);
}

void TemplateDialog::initView()
{
    //
    // Формируем модель стилей блоков (она для всех стилей едина)
    //
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::SceneHeading));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::SceneCharacters));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Action));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Character));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Parenthetical));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Dialogue));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Transition));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Note));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::TitleHeader));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Title));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::NoprintableText));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::FolderHeader));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::SceneDescription));
    m_ui->blockStyles->addItem(::makeListWidgetItem(ScenarioBlockStyle::Lyrics));
    m_ui->blockStyles->setAutoFillBackground(true);

    //
    // Сформируем модель из списка шрифтов системы
    //
    QStringListModel* fontsModel = new QStringListModel(m_ui->fontFamily);
    fontsModel->setStringList(QFontDatabase().families());
    m_ui->fontFamily->setModel(fontsModel);

    //
    // Предварительная настройка типа вертикальных отступов
    //
    aboutSpacingTypeChanged();
}

void TemplateDialog::initConnections()
{
    connect(m_ui->name, &QLineEdit::textChanged, [this] (const QString& _name) {
        if (!m_template.isDefault()) {
            setReadOnly(_name.isEmpty());
        }
    });

    connect(m_ui->blockStyles, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(aboutBlockStyleActivated(QListWidgetItem*)));
    connect(m_ui->spacingType, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutSpacingTypeChanged()));
    connect(m_ui->lineSpacing, SIGNAL(currentIndexChanged(int)), this, SLOT(aboutLineSpacingChanged()));

    connect(m_ui->buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_ui->buttons, SIGNAL(accepted()), this, SLOT(accept()));
}

void TemplateDialog::initStyleSheet()
{
    m_ui->blockStyles->setProperty("nobordersContainer", true);
    m_ui->scrollArea->setProperty("nobordersContainer", true);
}

void TemplateDialog::setReadOnly(bool _isReadOnly)
{
    m_ui->buttons->button(QDialogButtonBox::Save)->setEnabled(!_isReadOnly);
}
