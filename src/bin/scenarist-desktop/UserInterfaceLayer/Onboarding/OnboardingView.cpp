#include "OnboardingView.h"
#include "ui_OnboardingView.h"

OnboardingView::OnboardingView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnboardingView)
{
    ui->setupUi(this);
}

OnboardingView::~OnboardingView()
{
    delete ui;
}
