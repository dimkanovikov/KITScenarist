#include "CharactersDataEdit.h"
#include "ui_CharactersDataEdit.h"

#include <3rd_party/Helpers/TextEditHelper.h>

using UserInterface::CharactersDataEdit;


CharactersDataEdit::CharactersDataEdit(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CharactersDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

CharactersDataEdit::~CharactersDataEdit()
{
	delete ui;
}

void CharactersDataEdit::clean()
{
	removeConnections();

	m_sourceName.clear();
	m_sourceRealName.clear();
	m_sourceDescription.clear();
	m_sourcePhotos.clear();

	ui->sourceName->setText(tr("Choose Character"));
	ui->name->clear();
	ui->realName->clear();
	ui->description->clear();
	ui->photos->clear();

	initConnections();
}

void CharactersDataEdit::setName(const QString& _name)
{
	removeConnections();

	m_sourceName = _name;
	ui->sourceName->setText(_name);
	ui->name->setText(_name);

	initConnections();
}

QString CharactersDataEdit::name() const
{
	return ui->name->text();
}

void CharactersDataEdit::setRealName(const QString& _realName)
{
	removeConnections();

	m_sourceRealName = _realName;
	ui->realName->setText(_realName);

	initConnections();
}

QString CharactersDataEdit::realName() const
{
	return ui->realName->text();
}

void CharactersDataEdit::setDescription(const QString& _description)
{
	removeConnections();

	m_sourceDescription = _description;
	ui->description->setHtml(m_sourceDescription);

	initConnections();
}

QString CharactersDataEdit::description() const
{
	return TextEditHelper::removeDocumentTags(ui->description->toHtml());
}

void CharactersDataEdit::setPhotos(const QList<QPixmap>& _photos)
{
	removeConnections();

	m_sourcePhotos = _photos;
	ui->photos->setPhotos(_photos);

	//
	// Обновим состояние доступности кнопки добавления фотографий
	//
	bool addPhotoVisible = ui->photos->canAddPhoto();
	ui->addPhoto->setVisible(addPhotoVisible);

	initConnections();
}

QList<QPixmap> CharactersDataEdit::photos() const
{
	return ui->photos->photos();
}

namespace {
	static bool isEqualPixmapLists(const QList<QPixmap>& _lhs, const QList<QPixmap>& _rhs) {
		QList<QImage> lhs, rhs;
		foreach (const QPixmap& pixmap, _lhs) {
			lhs.append(pixmap.toImage());
		}
		foreach (const QPixmap& pixmap, _rhs) {
			rhs.append(pixmap.toImage());
		}
		return lhs == rhs;
	}
}

void CharactersDataEdit::aboutCharacterChanged()
{
	//
	// Сохраним персонаж, если изменены данные
	//
	if (name() != m_sourceName
		|| realName() != m_sourceRealName
		|| description() != m_sourceDescription
		|| !isEqualPixmapLists(photos(), m_sourcePhotos)) {
		emit saveCharacter();
	}

	//
	// Обновим состояние доступности кнопки добавления фотографий
	//
	bool addPhotoVisible = ui->photos->canAddPhoto();
	ui->addPhoto->setVisible(addPhotoVisible);
}

void CharactersDataEdit::initView()
{
	QFont nameFont = ui->name->font();
	nameFont.setCapitalization(QFont::AllUppercase);
	ui->name->setFont(nameFont);
}

void CharactersDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	connect(ui->realName, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutCharacterChanged()));
	connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutCharacterChanged()));

	connect(ui->addPhoto, SIGNAL(clicked()), ui->photos, SLOT(aboutAddPhoto()));
}

void CharactersDataEdit::removeConnections()
{
	disconnect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->realName, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutCharacterChanged()));

	disconnect(ui->addPhoto, SIGNAL(clicked()), ui->photos, SLOT(aboutAddPhoto()));
}

void CharactersDataEdit::initStyleSheet()
{
	ui->addPhoto->setProperty("inTopPanel", true);

	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
