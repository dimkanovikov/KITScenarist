#include "OnboardingView.h"
#include "ui_OnboardingView.h"

#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

#include <QDesktopWidget>
#include <QScreen>

using UserInterface::OnboardingView;


OnboardingView::OnboardingView(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::OnboardingView)
{
    m_ui->setupUi(this);

    initView();
    initConnections();
}

OnboardingView::~OnboardingView()
{
    delete m_ui;
}

bool OnboardingView::isUseDarkTheme() const
{
    return m_ui->darkTheme->isChecked();
}

bool OnboardingView::isUseResearch() const
{
    return m_ui->useResearch->isChecked();
}

bool OnboardingView::isUseCards() const
{
    return m_ui->useCards->isChecked();
}

bool OnboardingView::isUseScript() const
{
    return m_ui->useScript->isChecked();
}

QString OnboardingView::scriptTemplate() const
{
    for (const auto* selectedTemplate : m_ui->templates->findChildren<QRadioButton*>()) {
        if (selectedTemplate->isChecked()) {
            return selectedTemplate->text();
        }
    }

    return QString();
}

bool OnboardingView::isUseStatistics() const
{
    return m_ui->useStatistics->isChecked();
}

void OnboardingView::changeEvent(QEvent* _event)
{
    if (_event->type() == QEvent::LanguageChange) {
        m_ui->retranslateUi(this);
    } else {
        QWidget::changeEvent(_event);
    }
}

void OnboardingView::initView()
{
    //
    // Оставим только возможность закрытия окна
    //
    setWindowFlags(windowFlags() ^ Qt::WindowMinimizeButtonHint ^ Qt::WindowMaximizeButtonHint);

    //
    // Подвинем окно на центр экрана
    //
    const QRect screenRect = QApplication::desktop()->availableGeometry();
    const QPoint positionForShow((screenRect.width() - width()) / 2,
                     (screenRect.height() - height()) / 2);
    move(positionForShow);

    //
    // Настроим виджеты формы
    //
    m_ui->content->setCurrentWidget(m_ui->languagePage);
    m_ui->back->setEnabled(false);
}

void OnboardingView::initConnections()
{
    QButtonGroup* languagesGroup = new QButtonGroup(this);
    languagesGroup->addButton(m_ui->system);
    languagesGroup->addButton(m_ui->russian);
    languagesGroup->addButton(m_ui->spanish);
    languagesGroup->addButton(m_ui->english);
    languagesGroup->addButton(m_ui->french);
    languagesGroup->addButton(m_ui->kazakh);
    languagesGroup->addButton(m_ui->ukrainian);
    languagesGroup->addButton(m_ui->german);
    languagesGroup->addButton(m_ui->portugues);
    languagesGroup->addButton(m_ui->farsi);
    languagesGroup->addButton(m_ui->chinese);
    languagesGroup->addButton(m_ui->hebrew);
    connect(languagesGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this, &OnboardingView::notifyLanguageChange);
    connect(m_ui->darkTheme, &QRadioButton::toggled, this, &OnboardingView::useDarkThemeToggled);

    connect(m_ui->skip, &QPushButton::clicked, this, &OnboardingView::skipClicked);
    connect(m_ui->back, &QPushButton::clicked, this, &OnboardingView::goToPreviousPage);
    connect(m_ui->next, &QPushButton::clicked, this, &OnboardingView::goToNextPage);
}

void OnboardingView::notifyLanguageChange()
{
    int language = -1;
    if (m_ui->system->isChecked()) {
        language = -1;
    } else if (m_ui->russian->isChecked()) {
        language = 0;
    } else if (m_ui->spanish->isChecked()) {
        language = 1;
    } else if (m_ui->english->isChecked()) {
        language = 2;
    } else if (m_ui->french->isChecked()) {
        language = 3;
    } else if (m_ui->kazakh->isChecked()) {
        language = 4;
    } else if (m_ui->ukrainian->isChecked()) {
        language = 5;
    } else if (m_ui->german->isChecked()) {
        language = 6;
    } else if (m_ui->portugues->isChecked()) {
        language = 7;
    } else if (m_ui->farsi->isChecked()) {
        language = 8;
    } else if (m_ui->chinese->isChecked()) {
        language = 9;
    } else if (m_ui->hebrew->isChecked()) {
        language = 10;
    }

    emit languageChanged(language);
}

void OnboardingView::goToPreviousPage()
{
    int previousPageIndex = m_ui->content->currentIndex() - 1;
    //
    // Если предыщая страница - страница выбора шаблона скрипта, но использование скрипта отключено, пропускаем страницу с выбором шаблона
    //
    if (m_ui->content->currentWidget() == m_ui->statisticsPage
        && m_ui->useScript->isChecked() == false) {
        --previousPageIndex;
    }
    if (previousPageIndex >= 0) {
        //
        // Конфигурирем кнопки
        //
        m_ui->next->setText(tr("Next"));
        //
        // Если дошли до начала, то отключим кнопку возвращения назад
        //
        if (previousPageIndex == 0) {
            m_ui->back->setEnabled(false);
        }
        //
        // и перелистываем назад
        //
        WAF::StackedWidgetAnimation::slide(m_ui->content, m_ui->content->widget(previousPageIndex), WAF::FromLeftToRight);
    }
}

void OnboardingView::goToNextPage()
{
    int nextPageIndex = m_ui->content->currentIndex() + 1;
    //
    // Если текущая страница - страница включения скрипта и его использование отключено, пропускаем страницу с выбором шаблона
    //
    if (m_ui->content->currentWidget() == m_ui->scriptPage
        && m_ui->useScript->isChecked() == false) {
        ++nextPageIndex;
    }
    if (nextPageIndex < m_ui->content->count()) {
        //
        // Конфигурирем кнопки
        //
        m_ui->back->setEnabled(true);
        //
        // Если дошли до конца, покажем кнопку завершения мастера и скроем переход к следующему шагу
        //
        if ((nextPageIndex + 1) == m_ui->content->count()) {
            m_ui->next->setText(tr("Finish"));
        }
        //
        // И перелистываем вперёд
        //
        WAF::StackedWidgetAnimation::slide(m_ui->content, m_ui->content->widget(nextPageIndex), WAF::FromRightToLeft);
    } else {
        emit finishClicked();
    }
}
