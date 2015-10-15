#include "ScenarioTextView.h"
#include "ui_ScenarioTextView.h"

#include <QKeyEvent>
#include <QScroller>

using UserInterface::ScenarioTextView;


ScenarioTextView::ScenarioTextView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ScenarioTextView)
{
	m_ui->setupUi(this);

    initView();
    initConnections();
    initStyleSheet();
}

ScenarioTextView::~ScenarioTextView()
{
	delete m_ui;
}

QWidget* ScenarioTextView::toolbar() const
{
    return m_ui->toolbar;
}

void ScenarioTextView::initView()
{
    //
    // Отключаем автоподсказки
    //
    m_ui->textEdit->setInputMethodHints(m_ui->textEdit->inputMethodHints() | Qt::ImhNoPredictiveText);

    m_ui->textEdit->horizontalScrollBar()->hide();
    m_ui->textEdit->verticalScrollBar()->hide();

    QScroller::grabGesture(m_ui->textEdit);
}

void ScenarioTextView::initConnections()
{
    connect(m_ui->tab, &QToolButton::clicked, [=](){
       qApp->sendEvent(m_ui->textEdit, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
    });
}

void ScenarioTextView::initStyleSheet()
{

}
