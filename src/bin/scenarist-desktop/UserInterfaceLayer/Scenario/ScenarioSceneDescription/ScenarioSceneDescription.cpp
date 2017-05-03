#include "ScenarioSceneDescription.h"

#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditorWidget.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

using namespace UserInterface;


ScenarioSceneDescription::ScenarioSceneDescription(QWidget* _parent) :
    QWidget(_parent),
    m_titleHeader(new QFrame(this)),
    m_title(new QLineEdit(this)),
    m_description(new SimpleTextEditorWidget(this))
{
    initView();
    initConnections();
    initStyleSheet();
}

void ScenarioSceneDescription::setTitle(const QString& _title)
{
    const QString newTitle = _title.toUpper();
    if (currentTitle() != newTitle) {
        disconnect(m_title, &QLineEdit::textChanged, this, &ScenarioSceneDescription::aboutTitleChanged);

        m_sourceTitle = newTitle;

        m_title->setText(newTitle);
        m_title->setCursorPosition(0);
        m_title->setToolTip(newTitle);

        connect(m_title, &QLineEdit::textChanged, this, &ScenarioSceneDescription::aboutTitleChanged);
    }
}

void ScenarioSceneDescription::setDescription(const QString& _description)
{
    //
    // Если задан новый текст и мы не в режиме применения собственных изменений
    //
    if (m_inDescriptionChange == 0
        && currentDescription() != _description) {
        m_description->blockSignals(true);

        //
        // Сформируем значение описания, для корректности последующих сравнений
        //
        m_sourceDescription = _description;

        m_description->setPlainText(_description);

        m_description->blockSignals(false);
    }
}

void ScenarioSceneDescription::setCommentOnly(bool _isCommentOnly)
{
    m_title->setReadOnly(_isCommentOnly);
    m_description->setReadOnly(_isCommentOnly);
}

void ScenarioSceneDescription::aboutTitleChanged()
{
    if (m_sourceTitle != currentTitle()
        || currentTitle().isEmpty()) {
        emit titleChanged(currentTitle());
    }
}

void ScenarioSceneDescription::aboutDescriptionChanged()
{
    //
    // События от клавиатуры могут приходить достаточно часто, поэтому используем тут
    // числового охранника, чтобы быть уверенным, что все события были выполнены корректно
    //
    if (m_sourceDescription != currentDescription()
        || m_description->toPlainText().isEmpty()) {
        ++m_inDescriptionChange;
        emit descriptionChanged(m_description->toPlainText());
        --m_inDescriptionChange;
    }
}

QString ScenarioSceneDescription::currentTitle() const
{
    return m_title->text().toUpper().simplified();
}

QString ScenarioSceneDescription::currentDescription() const
{
    return m_description->toPlainText();
}

void ScenarioSceneDescription::initView()
{
    m_description->setToolbarVisible(false);

    QHBoxLayout* topLayout = new QHBoxLayout(m_titleHeader);
    topLayout->setContentsMargins(QMargins(6, 0, 6, 0));
    topLayout->addWidget(new QLabel(tr("Description of:"), this));
    topLayout->addWidget(m_title);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(m_titleHeader);
    layout->addWidget(m_description);

    setLayout(layout);
}

void ScenarioSceneDescription::initConnections()
{
    connect(m_title, &QLineEdit::textChanged, this, &ScenarioSceneDescription::aboutTitleChanged);
    connect(m_description, &SimpleTextEditorWidget::textChanged, this, &ScenarioSceneDescription::aboutDescriptionChanged);
}

void ScenarioSceneDescription::initStyleSheet()
{

    m_titleHeader->setProperty("inTopPanel", true);

    m_title->setProperty("editableLabel", true);

    m_description->setProperty("mainContainer", true);
}
