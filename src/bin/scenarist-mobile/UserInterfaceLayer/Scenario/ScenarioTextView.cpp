#include "ScenarioTextView.h"
#include "ui_ScenarioTextView.h"

using UserInterface::ScenarioTextView;


ScenarioTextView::ScenarioTextView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ScenarioTextView)
{
	m_ui->setupUi(this);
}

ScenarioTextView::~ScenarioTextView()
{
	delete m_ui;
}

QWidget* ScenarioTextView::toolbar() const
{
	return m_ui->toolbar;
}
