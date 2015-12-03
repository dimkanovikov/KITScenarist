#include "CharactersDataEdit.h"
#include "ui_CharactersDataEdit.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

using UserInterface::CharactersDataEdit;

namespace {
	/**
	 * @brief Получить путь к последней используемой папке с изображениями
	 */
	static QString imagesFolderPath() {
		QString imagesFolderPath =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"characters/images-folder",
					DataStorageLayer::SettingsStorage::ApplicationSettings);
		if (imagesFolderPath.isEmpty()) {
			imagesFolderPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		}
		return imagesFolderPath;
	}

	/**
	 * @brief Сохранить путь к последней используемой папке с изображениями
	 */
	static void saveImagesFolderPath(const QString& _path) {
		DataStorageLayer::StorageFacade::settingsStorage()->setValue(
					"characters/images-folder",
					QFileInfo(_path).absoluteDir().absolutePath(),
					DataStorageLayer::SettingsStorage::ApplicationSettings);
	}
}


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
	ui->sourceName->setText(_name);
	ui->name->setAcceptedText(_name);
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

void CharactersDataEdit::setCommentOnly(bool _isCommentOnly)
{
	ui->addPhoto->setVisible(!_isCommentOnly);
	ui->name->setReadOnly(_isCommentOnly);
	ui->realName->setReadOnly(_isCommentOnly);
	ui->description->setReadOnly(_isCommentOnly);
}

void CharactersDataEdit::aboutCharacterChanged()
{
	//
	// Сохраним персонаж
	//
	emit saveCharacter();

	//
	// Обновим состояние доступности кнопки добавления фотографий
	//
	bool addPhotoVisible = ui->photos->canAddPhoto();
	ui->addPhoto->setVisible(addPhotoVisible);
}

void CharactersDataEdit::aboutAddPhoto()
{
	const QString newImagesFolder = ui->photos->aboutAddPhoto(::imagesFolderPath());
	if (!newImagesFolder.isEmpty()) {
		::saveImagesFolderPath(newImagesFolder);
	}
}

void CharactersDataEdit::initView()
{
	ui->addPhoto->updateIcons();

	QFont nameFont = ui->name->font();
	nameFont.setCapitalization(QFont::AllUppercase);
	ui->name->setFont(nameFont);
	ui->name->setQuestionPrefix(tr("Character name"));
}

void CharactersDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textAccepted(QString,QString)), this, SLOT(aboutCharacterChanged()));
	connect(ui->realName, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutCharacterChanged()));
	connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutCharacterChanged()));

	connect(ui->addPhoto, SIGNAL(clicked()), this, SLOT(aboutAddPhoto()));
}

void CharactersDataEdit::removeConnections()
{
	disconnect(ui->name, SIGNAL(textAccepted(QString,QString)), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->realName, SIGNAL(textChanged(QString)), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutCharacterChanged()));
	disconnect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutCharacterChanged()));

	disconnect(ui->addPhoto, SIGNAL(clicked()), this, SLOT(aboutAddPhoto()));
}

void CharactersDataEdit::initStyleSheet()
{
	ui->addPhoto->setProperty("inTopPanel", true);

	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
