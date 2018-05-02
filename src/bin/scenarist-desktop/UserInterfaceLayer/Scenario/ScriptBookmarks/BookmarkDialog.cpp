#include "BookmarkDialog.h"
#include "ui_BookmarkDialog.h"

using UserInterface::BookmarkDialog;


BookmarkDialog::BookmarkDialog(QWidget* _parent, bool _isNew) :
    QLightBoxDialog(_parent),
    m_ui(new Ui::BookmarkDialog)
{
    m_ui->setupUi(this);
    m_addButton = m_ui->buttons->addButton(_isNew ? tr("Add new bookmark") : tr("Update bookmark"),
                                           QDialogButtonBox::AcceptRole);
    m_ui->bookmarkColor->setColorsPane(ColoredToolButton::Google);
    m_ui->bookmarkColor->setColor(Qt::yellow);

    initStyleSheet();
}

BookmarkDialog::~BookmarkDialog()
{
    delete m_ui;
}

QString BookmarkDialog::bookmarkText() const
{
    return m_ui->bookmarkText->text();
}

void BookmarkDialog::setBookmarkText(const QString& _text)
{
    m_ui->bookmarkText->setText(_text);
}

QColor BookmarkDialog::bookmarkColor() const
{
    return m_ui->bookmarkColor->currentColor();
}

void BookmarkDialog::setBookmarkColor(const QColor& _color)
{
    m_ui->bookmarkColor->setColor(_color);
}

QWidget* BookmarkDialog::focusedOnExec() const
{
    return m_ui->bookmarkText;
}

void BookmarkDialog::initView()
{
}

void BookmarkDialog::initConnections()
{
    connect(m_ui->buttons, &QDialogButtonBox::accepted, this, &BookmarkDialog::accept);
    connect(m_ui->buttons, &QDialogButtonBox::rejected, this, &BookmarkDialog::reject);
}

void BookmarkDialog::initStyleSheet()
{
    m_ui->bookmarkColor->setProperty("itemDialog", true);
}
