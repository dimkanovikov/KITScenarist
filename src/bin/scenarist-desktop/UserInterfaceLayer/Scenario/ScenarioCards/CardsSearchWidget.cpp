#include "CardsSearchWidget.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>


CardsSearchWidget::CardsSearchWidget(QWidget* _parent)
    : QFrame(_parent),
      m_searchText(new QLineEdit(this)),
      m_caseSensitive(new QPushButton(this)),
      m_searchEverywhere(new QPushButton(this)),
      m_searchInText(new QPushButton(this)),
      m_searchInTags(new QPushButton(this))
{
    initView();
    initStyleSheet();
    initConnections();
}

void CardsSearchWidget::selectText()
{
    m_searchText->selectAll();

    notifySearchRequested();
}

void CardsSearchWidget::initView()
{
    setFocusProxy(m_searchText);
    m_searchText->setPlaceholderText(tr("Find..."));
    m_searchText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_caseSensitive->setFixedWidth(20);
    m_caseSensitive->setCheckable(true);
    m_caseSensitive->setText("Aa");
    QFont caseSensitiveFont = m_caseSensitive->font();
    caseSensitiveFont.setItalic(true);
    m_caseSensitive->setFont(caseSensitiveFont);
    m_caseSensitive->setToolTip(tr("Case Sensitive"));

    QButtonGroup* buttons = new QButtonGroup(this);
    buttons->addButton(m_searchEverywhere);
    buttons->addButton(m_searchInText);
    buttons->addButton(m_searchInTags);
    m_searchEverywhere->setCheckable(true);
    m_searchEverywhere->setText(tr("Everywhere"));
    m_searchEverywhere->setChecked(true);
    m_searchInText->setCheckable(true);
    m_searchInText->setText(tr("In text"));
    m_searchInTags->setCheckable(true);
    m_searchInTags->setText(tr("In tags"));

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->addWidget(m_searchText);
    layout->addWidget(m_caseSensitive);
    layout->addSpacing(16);
    layout->addWidget(m_searchEverywhere);
    layout->addWidget(m_searchInText);
    layout->addWidget(m_searchInTags);

    setLayout(layout);
}

void CardsSearchWidget::initStyleSheet()
{
    setFrameShape(QFrame::Box);
    QString styleSheet = "*[searchWidget=\"true\"] {"
                         "  border: 0px solid black; "
                         "  border-top-width: 1px; "
                         "  border-top-style: solid; "
                         "  border-top-color: palette(dark);"
                         "}";
    if (QLocale().textDirection() == Qt::LeftToRight) {
        styleSheet += "*[middle=\"true\"] { border-left: 0; min-width: 20px; }"
                      "*[last=\"true\"] { border-left: 0; min-width: 20px; }";
    } else {
        styleSheet += "*[middle=\"true\"] { border-right: 0; min-width: 20px; }"
                      "*[last=\"true\"] { border-right: 0; min-width: 20px; }";
    }
    setStyleSheet(styleSheet);
    setProperty("searchWidget", true);
    m_caseSensitive->setProperty("last", true);
    m_searchEverywhere->setProperty("middle", true);
    m_searchInText->setProperty("middle", true);
    m_searchInTags->setProperty("last", true);
}

void CardsSearchWidget::initConnections()
{
    connect(m_searchText, &QLineEdit::textChanged, this, &CardsSearchWidget::notifySearchRequested);
    connect(m_caseSensitive, &QPushButton::toggled, this, &CardsSearchWidget::notifySearchRequested);
    connect(m_searchEverywhere, &QPushButton::toggled, this, &CardsSearchWidget::notifySearchRequested);
    connect(m_searchInText, &QPushButton::toggled, this, &CardsSearchWidget::notifySearchRequested);
    connect(m_searchInTags, &QPushButton::toggled, this, &CardsSearchWidget::notifySearchRequested);
}

void CardsSearchWidget::notifySearchRequested()
{
    emit searchRequested(m_searchText->text(),
                         m_caseSensitive->isChecked(),
                         m_searchEverywhere->isChecked() || m_searchInText->isChecked(),
                         m_searchEverywhere->isChecked() || m_searchInTags->isChecked());
}
