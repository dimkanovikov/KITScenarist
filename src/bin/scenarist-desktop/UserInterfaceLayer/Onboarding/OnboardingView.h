#ifndef ONBOARDINGVIEW_H
#define ONBOARDINGVIEW_H

#include <QWidget>

namespace Ui {
    class OnboardingView;
}

class OnboardingView : public QWidget
{
    Q_OBJECT

public:
    explicit OnboardingView(QWidget *parent = 0);
    ~OnboardingView();

private:
    Ui::OnboardingView *ui;
};

#endif // ONBOARDINGVIEW_H
