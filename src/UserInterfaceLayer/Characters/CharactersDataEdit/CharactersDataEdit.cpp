#include "CharactersDataEdit.h"
#include "ui_CharactersDataEdit.h"

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
	m_sourceName.clear();
	m_sourceRealName.clear();
	m_sourceDescription.clear();
	m_sourcePhotos.clear();

	ui->sourceName->setText(tr("Choose Character"));
	ui->name->clear();
	ui->realName->clear();
	ui->description->clear();
	ui->photos->clear();
	initView();
}

void CharactersDataEdit::setName(const QString& _name)
{
	m_sourceName = _name;

	ui->sourceName->setText(_name);
	ui->name->setText(_name);
}

QString CharactersDataEdit::name() const
{
	return ui->name->text();
}

void CharactersDataEdit::setRealName(const QString& _realName)
{
	m_sourceRealName = _realName;

	ui->realName->setText(_realName);
}

QString CharactersDataEdit::realName() const
{
	return ui->realName->text();
}

void CharactersDataEdit::setDescription(const QString& _description)
{
	m_sourceDescription = _description;

	ui->description->setHtml(_description);
}

QString CharactersDataEdit::description() const
{
	return ui->description->toPlainText().isEmpty() ? QString() : ui->description->toHtml();
}

void CharactersDataEdit::setPhotos(const QList<QPixmap>& _photos)
{
	m_sourcePhotos = _photos;

	ui->photos->setPhotos(_photos);
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

void CharactersDataEdit::updateState()
{
	//
	// Обновим состояние доступности кнопки сохранения
	//
	bool saveEnabled = false;
	if (name() != m_sourceName
		|| realName() != m_sourceRealName
		|| description() != m_sourceDescription
		|| !isEqualPixmapLists(photos(), m_sourcePhotos)) {
		saveEnabled = true;
	}
	ui->save->setEnabled(saveEnabled);

	//
	// Обновим состояние доступности кнопки добавления фотографий
	//
	bool addPhotoEnabled = ui->photos->canAddPhoto();
	ui->addPhoto->setEnabled(addPhotoEnabled);
}

void CharactersDataEdit::initView()
{
}

void CharactersDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(updateState()));
	connect(ui->realName, SIGNAL(textChanged(QString)), this, SLOT(updateState()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(updateState()));
	connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(updateState()));

	connect(ui->addPhoto, SIGNAL(clicked()), ui->photos, SLOT(aboutAddPhoto()));
	connect(ui->save, SIGNAL(clicked()), this, SIGNAL(saveCharacter()));
	connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(reloadCharacter()));
}

void CharactersDataEdit::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
