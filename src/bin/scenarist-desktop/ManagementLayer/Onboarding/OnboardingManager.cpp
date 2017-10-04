#include "OnboardingManager.h"

#include <UserInterfaceLayer/Onboarding/OnboardingView.h>

using ManagementLayer::OnboardingManager;
using UserInterface::OnboardingView;


OnboardingManager::OnboardingManager(QObject* _parent) :
    QObject(_parent),
    m_view(new OnboardingView)
{
    initConnections();
}

OnboardingManager::~OnboardingManager()
{
    m_view->deleteLater();
}

bool OnboardingManager::needConfigureApp() const
{
    return true;
}

void OnboardingManager::exec()
{
    m_view->show();
}

void OnboardingManager::initConnections()
{
    connect(m_view, &OnboardingView::finishClicked, m_view, &OnboardingView::close);
    connect(m_view, &OnboardingView::finishClicked, this, &OnboardingManager::finished);
}
