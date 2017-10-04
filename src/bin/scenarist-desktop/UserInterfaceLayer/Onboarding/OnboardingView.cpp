#include "OnboardingView.h"
#include "ui_OnboardingView.h"

using UserInterface::OnboardingView;


OnboardingView::OnboardingView(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::OnboardingView)
{
    m_ui->setupUi(this);

    initConnections();
}

OnboardingView::~OnboardingView()
{
    delete m_ui;
}

void OnboardingView::initConnections()
{
    connect(m_ui->finish, &QPushButton::clicked, this, &OnboardingView::finishClicked);
}
