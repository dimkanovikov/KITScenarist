#include "CardsResizer.h"
#include "ui_CardsResizer.h"

using UserInterface::CardsResizer;


CardsResizer::CardsResizer(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::CardsResizer)
{
    m_ui->setupUi(this);

    initView();
    initConnections();
}

CardsResizer::~CardsResizer()
{
    delete m_ui;
}

int CardsResizer::cardSize() const
{
    return m_ui->cardSize->value();
}

int CardsResizer::cardRatio() const
{
    return m_ui->cardRatio->value();
}

int CardsResizer::distance() const
{
    return m_ui->distance->value();
}

int CardsResizer::cardsInRow() const
{
    if (m_ui->arrangeCards->isChecked())
        return m_ui->cardsInLine->value();

    //
    // Автоматическая сортировка
    //
    return 0;
}

void CardsResizer::initView()
{

}

void CardsResizer::initConnections()
{
    connect(m_ui->arrangeCards, &QCheckBox::toggled, m_ui->cardsInLine, &QSpinBox::setEnabled);
    connect(m_ui->arrangeCards, &QCheckBox::toggled, m_ui->cardsInLineLabel, &QSpinBox::setEnabled);

    connect(m_ui->cardSize, &QSlider::valueChanged, this, &CardsResizer::parametersChanged);
    connect(m_ui->cardRatio, &QSlider::valueChanged, this, &CardsResizer::parametersChanged);
    connect(m_ui->distance, &QSlider::valueChanged, this, &CardsResizer::parametersChanged);
    connect(m_ui->arrangeCards, &QCheckBox::toggled, this, &CardsResizer::parametersChanged);
    connect(m_ui->cardsInLine, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &CardsResizer::parametersChanged);
}
