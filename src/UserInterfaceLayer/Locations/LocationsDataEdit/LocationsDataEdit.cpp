#include "LocationsDataEdit.h"
#include "ui_LocationsDataEdit.h"

using UserInterface::LocationsDataEdit;


LocationsDataEdit::LocationsDataEdit(QWidget *parent) :
	QDialog(parent),
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
	m_sourceName.clear();
	m_sourceDescription.clear();
	m_sourcePhotos.clear();

	ui->sourceName->setText(tr("Choose Location"));
	ui->name->clear();
	ui->description->clear();
	ui->photos->clear();
}

void LocationsDataEdit::setName(const QString& _name)
{
	m_sourceName = _name;

	ui->sourceName->setText(_name);
	ui->name->setText(_name);
}

QString LocationsDataEdit::name() const
{
	return ui->name->text().toUpper();
}

void LocationsDataEdit::setDescription(const QString& _description)
{
	m_sourceDescription = _description;

	ui->description->setHtml(_description);
}

QString LocationsDataEdit::description() const
{
	return ui->description->toPlainText().isEmpty() ? QString() : ui->description->toHtml();
}

void LocationsDataEdit::setPhotos(const QList<QPixmap>& _photos)
{
	m_sourcePhotos = _photos;

	ui->photos->setPhotos(_photos);
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

void LocationsDataEdit::updateSaveState()
{
	bool saveEnabled = false;
	if (name() != m_sourceName
		|| description() != m_sourceDescription
		|| !isEqualPixmapLists(photos(), m_sourcePhotos)) {
		saveEnabled = true;
	}

	ui->save->setEnabled(saveEnabled);
}

void LocationsDataEdit::initView()
{

}

void LocationsDataEdit::initConnections()
{
	connect(ui->name, SIGNAL(textChanged(QString)), this, SLOT(updateSaveState()));
	connect(ui->description, SIGNAL(textChanged()), this, SLOT(updateSaveState()));
	connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(updateSaveState()));

	connect(ui->save, SIGNAL(clicked()), this, SIGNAL(saveLocation()));
	connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(reloadLocation()));
}

void LocationsDataEdit::initStyleSheet()
{
	ui->topEmptyLabel->setProperty("inTopPanel", true);
	ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
	ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

	ui->mainContainer->setProperty("mainContainer", true);
}
