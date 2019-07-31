#include "ResearchItemDialog.h"
#include "ui_ResearchItemDialog.h"

#include <Domain/Research.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QPushButton>

using UserInterface::ResearchItemDialog;


ResearchItemDialog::ResearchItemDialog(QWidget* _parent) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::ResearchItemDialog)
{
    m_ui->setupUi(this);
    m_saveButton = m_ui->buttons->addButton(tr("Add"), QDialogButtonBox::AcceptRole);
}

ResearchItemDialog::~ResearchItemDialog()
{
    delete m_ui;
}

void ResearchItemDialog::clear()
{
    m_ui->name->clear();
    m_saveButton->setEnabled(false);
}

void ResearchItemDialog::setInsertParent(const QString& _parentName)
{
    if (!_parentName.isEmpty()) {
        m_ui->isInsert->show();
        m_ui->isInsert->setText(tr("Insert into \"%1\"").arg(_parentName));
    } else {
        m_ui->isInsert->hide();
        m_ui->isInsert->setText(QString::null);
    }
}

void ResearchItemDialog::setInsertAllow(bool _isCharacterAllow, bool _isLocationAllow)
{
    disconnect(m_ui->character, &QRadioButton::toggled, m_ui->isInsert, &QCheckBox::toggle);
    disconnect(m_ui->location, &QRadioButton::toggled, m_ui->isInsert, &QCheckBox::toggle);

    //
    // Настроим видимость вариантов выбора персонажей и локаций
    //
    m_ui->character->setVisible(_isCharacterAllow);
    m_ui->location->setVisible(_isLocationAllow);

    if (_isCharacterAllow || _isLocationAllow) {
        connect(m_ui->character, &QRadioButton::toggled, m_ui->isInsert, &QCheckBox::toggle);
        connect(m_ui->location, &QRadioButton::toggled, m_ui->isInsert, &QCheckBox::toggle);
        m_ui->isInsert->setChecked(false);
        m_ui->isInsert->setEnabled(false);
    } else {
        m_ui->isInsert->setEnabled(true);
    }

    //
    // Настроим видимость вариантов выбора данных
    //
    bool isDataItemsVisible = true;
    if (_isCharacterAllow || _isLocationAllow) {
        isDataItemsVisible = !m_ui->isInsert->text().isNull();
    }
    m_ui->folder->setVisible(isDataItemsVisible);
    m_ui->text->setVisible(isDataItemsVisible);
    m_ui->other->setVisible(isDataItemsVisible);
    m_ui->otherType->setVisible(isDataItemsVisible);

    //
    // Если выбран видимый в данный момент элемент
    //
    if (isDataItemsVisible
        && (m_ui->folder->isChecked()
            || m_ui->text->isChecked()
            || m_ui->other->isChecked())) {
        //
        // ... то всё ок, так и оставляем
        //

        if (_isCharacterAllow || _isLocationAllow) {
            m_ui->isInsert->setChecked(true);
        }
    }
    //
    // А если невидимый
    //
    else {
        //
        // ... то выберем один из видимых
        //
        if (_isCharacterAllow) {
            m_ui->character->setChecked(true);
            m_ui->isInsert->setChecked(false);
        } else {
            if (_isLocationAllow) {
                m_ui->location->setChecked(true);
                m_ui->isInsert->setChecked(false);
            } else {
                m_ui->folder->setChecked(true);
            }
        }
    }
}

void ResearchItemDialog::setResearchType(int _type)
{
    switch (_type) {
        case Domain::Research::Character: {
            m_ui->character->setChecked(true);
            break;
        }

        case Domain::Research::Location: {
            m_ui->location->setChecked(true);
            break;
        }

        case Domain::Research::Folder: {
            m_ui->folder->setChecked(true);
            break;
        }

        case Domain::Research::Text: {
            m_ui->text->setChecked(true);
            break;
        }

        case Domain::Research::MindMap: {
            m_ui->other->setChecked(true);
            m_ui->otherType->setCurrentIndex(0);
            break;
        }

        case Domain::Research::ImagesGallery: {
            m_ui->other->setChecked(true);
            m_ui->otherType->setCurrentIndex(1);
            break;
        }

        case Domain::Research::Url: {
            m_ui->other->setChecked(true);
            m_ui->otherType->setCurrentIndex(2);
            break;
        }

        default: break;
    }
}

int ResearchItemDialog::researchType() const
{
    Domain::Research::Type type = Domain::Research::Text;
    if (m_ui->character->isChecked()) {
        type = Domain::Research::Character;
    } else if (m_ui->location->isChecked()) {
        type = Domain::Research::Location;
    } else if (m_ui->folder->isChecked()) {
        type = Domain::Research::Folder;
    } else if (m_ui->text->isChecked()) {
        type = Domain::Research::Text;
    } else if (m_ui->other->isChecked()) {
        switch (m_ui->otherType->currentIndex()) {
            case 0: {
                type = Domain::Research::MindMap;
                break;
            }

            case 1: {
                type = Domain::Research::ImagesGallery;
                break;
            }

            case 2: {
                type = Domain::Research::Url;
                break;
            }
        }
    }
    return type;
}

QString ResearchItemDialog::researchName() const
{
    if (m_ui->character->isChecked()
        || m_ui->location->isChecked()) {
        return TextEditHelper::smartToUpper(m_ui->name->text());
    }

    return m_ui->name->text();
}

bool ResearchItemDialog::insertResearchInParent() const
{
    return !m_ui->isInsert->text().isEmpty() && m_ui->isInsert->isChecked();
}

QWidget* ResearchItemDialog::focusedOnExec() const
{
    return m_ui->name;
}

void ResearchItemDialog::initView()
{
    m_ui->otherType->setEnabled(m_ui->other->isChecked());

#ifdef Q_OS_WIN
    //
    // В виндовс XP webengine не работает, поэтому скрываем пункт с добавлением ссылки
    //
    if (QSysInfo::windowsVersion() == QSysInfo::WV_XP) {
        m_ui->otherType->removeItem(2);
    }
#endif
}

void ResearchItemDialog::initConnections()
{
    connect(m_ui->other, &QRadioButton::toggled, m_ui->otherType, &QComboBox::setEnabled);

    connect(m_ui->name, &QLineEdit::textChanged, this, [this] (const QString& _text) {
        m_saveButton->setEnabled(!_text.simplified().isEmpty());
    });
    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &QLightBoxDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &QLightBoxDialog::reject);
}
