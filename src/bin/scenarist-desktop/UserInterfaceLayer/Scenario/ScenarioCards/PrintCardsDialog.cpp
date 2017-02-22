#include "PrintCardsDialog.h"
#include "ui_PrintCardsDialog.h"

#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

using UserInterface::PrintCardsDialog;


PrintCardsDialog::PrintCardsDialog(QWidget *parent) :
    QLightBoxDialog(parent),
    m_ui(new Ui::PrintCardsDialog)
{
    m_ui->setupUi(this);

    initView();
    initConnections();
}

PrintCardsDialog::~PrintCardsDialog()
{
    delete m_ui;
}

void PrintCardsDialog::initView()
{
    m_ui->layoutsStack->setCurrentWidget(m_ui->pageP1);

    QLightBoxDialog::initView();
}

void PrintCardsDialog::initConnections()
{
    auto changeLayoutSample = [=] {
        QWidget* layout = nullptr;
        if (m_ui->portrait->isChecked()) {
            if (m_ui->oneCard->isChecked()) {
                layout = m_ui->pageP1;
            } else if (m_ui->twoCards->isChecked()) {
                layout = m_ui->pageP2;
            } else if (m_ui->fourCards->isChecked()) {
                layout = m_ui->pageP4;
            } else if (m_ui->sixCards->isChecked()) {
                layout = m_ui->pageP6;
            } else {
                layout = m_ui->pageP8;
            }
        } else {
            if (m_ui->oneCard->isChecked()) {
                layout = m_ui->pageL1;
            } else if (m_ui->twoCards->isChecked()) {
                layout = m_ui->pageL2;
            } else if (m_ui->fourCards->isChecked()) {
                layout = m_ui->pageL4;
            } else if (m_ui->sixCards->isChecked()) {
                layout = m_ui->pageL6;
            } else {
                layout = m_ui->pageL8;
            }
        }

        WAF::StackedWidgetAnimation::fadeIn(m_ui->layoutsStack, layout);
    };
    connect(m_ui->oneCard, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->twoCards, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->fourCards, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->sixCards, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->eightCards, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->portrait, &QRadioButton::toggled, changeLayoutSample);
    connect(m_ui->landscape, &QRadioButton::toggled, changeLayoutSample);

    connect(m_ui->cancel, &QPushButton::clicked, this, &PrintCardsDialog::reject);
    connect(m_ui->printPreview, &QPushButton::clicked, [=] {
        int cardsCount = 1;
        if (m_ui->oneCard->isChecked()) {
            cardsCount = 1;
        } else if (m_ui->twoCards->isChecked()) {
            cardsCount = 2;
        } else if (m_ui->fourCards->isChecked()) {
            cardsCount = 4;
        } else if (m_ui->sixCards->isChecked()) {
            cardsCount = 6;
        } else {
            cardsCount = 8;
        }
        bool isPortrait = m_ui->portrait->isChecked();
        emit printPreview(cardsCount, isPortrait);
    });

    QLightBoxDialog::initConnections();
}
