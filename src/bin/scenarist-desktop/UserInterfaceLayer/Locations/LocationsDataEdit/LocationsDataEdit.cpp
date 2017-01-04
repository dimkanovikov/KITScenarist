#include "LocationsDataEdit.h"
#include "ui_LocationsDataEdit.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

using UserInterface::LocationsDataEdit;

namespace {
    /**
     * @brief Получить путь к последней используемой папке с изображениями
     */
    static QString imagesFolderPath() {
        QString imagesFolderPath =
                DataStorageLayer::StorageFacade::settingsStorage()->value(
                    "locations/images-folder",
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
                    "locations/images-folder",
                    QFileInfo(_path).absoluteDir().absolutePath(),
                    DataStorageLayer::SettingsStorage::ApplicationSettings);
    }
}


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
    ui->sourceName->setText(_name);
    ui->name->setAcceptedText(_name);
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
    bool addPhotoVisible = ui->photos->canAddPhoto() && !ui->name->isReadOnly();
    ui->addPhoto->setVisible(addPhotoVisible);

    initConnections();
}

QList<QPixmap> LocationsDataEdit::photos() const
{
    return ui->photos->photos();
}

void LocationsDataEdit::setCommentOnly(bool _isCommentOnly)
{
    ui->addPhoto->setVisible(!_isCommentOnly);
    ui->name->setReadOnly(_isCommentOnly);
    ui->description->setReadOnly(_isCommentOnly);
}

void LocationsDataEdit::aboutLocationChanged()
{
    //
    // Сохраним локацию
    //
    emit saveLocation();

    //
    // Обновим состояние доступности кнопки добавления фотографий
    //
    bool addPhotoVisible = ui->photos->canAddPhoto();
    ui->addPhoto->setVisible(addPhotoVisible);
}

void LocationsDataEdit::aboutAddPhoto()
{
    const QString newImagesFolder = ui->photos->aboutAddPhoto(::imagesFolderPath());
    if (!newImagesFolder.isEmpty()) {
        ::saveImagesFolderPath(newImagesFolder);
    }
}

void LocationsDataEdit::initView()
{
    ui->addPhoto->updateIcons();

    QFont nameFont = ui->name->font();
    nameFont.setCapitalization(QFont::AllUppercase);
    ui->name->setFont(nameFont);
    ui->name->setQuestionPrefix(tr("Location"));
}

void LocationsDataEdit::initConnections()
{
    connect(ui->name, SIGNAL(textAccepted(QString,QString)), this, SLOT(aboutLocationChanged()));
    connect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutLocationChanged()));
    connect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutLocationChanged()));

    connect(ui->addPhoto, SIGNAL(clicked()), this, SLOT(aboutAddPhoto()));
}

void LocationsDataEdit::removeConnections()
{
    disconnect(ui->name, SIGNAL(textAccepted(QString,QString)), this, SLOT(aboutLocationChanged()));
    disconnect(ui->description, SIGNAL(textChanged()), this, SLOT(aboutLocationChanged()));
    disconnect(ui->photos, SIGNAL(photoChanged()), this, SLOT(aboutLocationChanged()));

    disconnect(ui->addPhoto, SIGNAL(clicked()), this, SLOT(aboutAddPhoto()));
}

void LocationsDataEdit::initStyleSheet()
{
    ui->addPhoto->setProperty("inTopPanel", true);

    ui->topEmptyLabel->setProperty("inTopPanel", true);
    ui->topEmptyLabel->setProperty("topPanelTopBordered", true);
    ui->topEmptyLabel->setProperty("topPanelRightBordered", true);

    ui->mainContainer->setProperty("mainContainer", true);
}
