#include "LocationsDataEdit.h"
#include "ui_LocationsDataEdit.h"

#include <3rd_party/Helpers/TextEditHelper.h>

using UserInterface::LocationsDataEdit;


LocationsDataEdit::LocationsDataEdit(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LocationsDataEdit)
{
	ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

LocationsDataEdit::~LocationsDataEdit()
{
	delete ui;
}

void LocationsDataEdit::clean()
{
	removeConnections();

	m_sourceName.clear();
	m_sourceDescription.clear();
	m_sourcePhotos.clear();

	ui->sourceName->setText(tr("Choose Location"));
	ui->name->clear();
	ui->description->clear();
	ui->photos->clear();

	initConnections();
}

void LocationsDataEdit::setName(const QString& _name)
{
	removeConnections();

	m_sourceName = _name;
	ui->sourceName->setText(_name);
	ui->name->setText(_name);

	initConnections();
}

QString LocationsDataEdit::name() const
{
	return ui->name->text().toUpper();
}

void LocationsDataEdit::setDescription(const QString& _description)
{
	removeConnections();

	m_sourceDescription = _description;
	ui->description->setHtml(m_sourceDescription);

	initConnections();
}

QString LocationsDataEdit::description() const
{
	return TextEditHelper::removeDocumentTags(ui->description->toHtml());
}

void LocationsDataEdit::setPhotos(const QList<QPixmap>& _photos)
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

QList<QPixmap> LocationsDataEdit::photos() const
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

void LocationsDataEdit::aboutLocationChanged()
{
	//
	// Сохраним локацию, если она была изменена
	//
	if (name() != m_sourceName
		|| description() != m_sourceDescription
		|| !isEqualPixmapLists(photos(), m_sourcePhotos)) {
		emit saveLocation();
	}

	//
	// Обновим состояние доступности кнопки добавления фотографий
	//
	bool addPhotoVisible = ui->photos->canAddPhoto();
	ui->addPhoto->setVisible(addPhotoVisible);
}

void LocationsDataEdit::initView()
{
	QFont nameFont = ui->name->font();
	nameFont.setCapitalization(QFont::AllUppercase);
	ui->name->setFont(nameFont);
}

void LocationsDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutLocationChanged()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutLocationChanged()));
	connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutLocationChanged()));

	connect(ui->addPhoto, SIGNAL(clicked()), ui->photos, SLOT(aboutAddPhoto()));
}

void LocationsDataEdit::removeConnections()
{
	disconnect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(aboutLocationChanged()));
	disconnect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutLocationChanged()));
	disconnect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutLocationChanged()));

	disconnect(ui->addPhoto, SIGNAL(clicked()), ui->photos, SLOT(aboutAddPhoto()));
}

void LocationsDataEdit::initStyleSheet()
{
	ui->addPhoto->setProperty("inTopPanel", true);

	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
