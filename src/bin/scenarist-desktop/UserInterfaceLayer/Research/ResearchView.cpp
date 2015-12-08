#include "ResearchView.h"
#include "ui_ResearchView.h"

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <3rd_party/Helpers/TextEditHelper.h>

#include "ResearchNavigatorItemDelegate.h"
#include "ResearchNavigatorProxyStyle.h"

#include <QFileDialog>
#include <QStandardPaths>

using UserInterface::ResearchView;
using UserInterface::ResearchNavigatorItemDelegate;
using UserInterface::ResearchNavigatorProxyStyle;

namespace {
	/**
	 * @brief Получить путь к последней используемой папке с изображениями
	 */
	static QString imagesFolderPath() {
		QString imagesFolderPath =
				DataStorageLayer::StorageFacade::settingsStorage()->value(
					"research/images-folder",
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
		QFileInfo info(_path);

		DataStorageLayer::StorageFacade::settingsStorage()->setValue(
					"research/images-folder",
					info.isFile() ? info.absoluteDir().absolutePath() : _path,
					DataStorageLayer::SettingsStorage::ApplicationSettings);
	}
}


ResearchView::ResearchView(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::ResearchView)
{
	m_ui->setupUi(this);

	initView();
	initConnections();
	initStyleSheet();
}

ResearchView::~ResearchView()
{
	delete m_ui;
}

void ResearchView::setResearchModel(QAbstractItemModel* _model)
{
	//
	// Загружаем модель
	//
	m_ui->researchNavigator->setModel(_model);
	if (_model != 0) {
		m_ui->researchNavigator->expand(_model->index(0, 0));
		m_ui->researchNavigator->expand(_model->index(1, 0));
		//
		// Выбираем сценарий
		//
		selectItem(_model->index(0, 0));

		//
		// Настраиваем соединения
		//
		connect(m_ui->researchNavigator->selectionModel(), &QItemSelectionModel::selectionChanged,
				this, &ResearchView::currentResearchChanged);
	}
}

QModelIndex ResearchView::currentResearchIndex() const
{
	QModelIndex currentResearchIndex;
	foreach (QModelIndex researchIndex, m_ui->researchNavigator->selectionModel()->selectedIndexes()) {
		currentResearchIndex = researchIndex;
		break;
	}
	return currentResearchIndex;
}

void ResearchView::selectItem(const QModelIndex& _index)
{
	m_ui->researchNavigator->setCurrentIndex(_index);
}

void ResearchView::editScenario(const QString& _name, const QString& _logline)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->scenarioEdit);
	m_ui->scenarioName->setText(_name);
	m_ui->scenarioLogline->setText(_logline);

	setResearchManageButtonsVisible(false);
	setSearchVisible(false);
}

void ResearchView::editTitlePage(const QString& _name, const QString& _additionalInfo,
	const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->titlePageEdit);
	m_ui->titlePageName->setText(_name);
	m_ui->titlePageAdditionalInfo->setEditText(_additionalInfo);
	m_ui->titlePageGenre->setText(_genre);
	m_ui->titlePageAuthor->setText(_author);
	m_ui->titlePageContacts->setPlainText(_contacts);
	m_ui->titlePageYear->setText(_year);

	setResearchManageButtonsVisible(false);
	setSearchVisible(false);
}

void ResearchView::editSynopsis(const QString& _synopsis)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->synopsisEdit);
	m_ui->synopsisText->setHtml(_synopsis);

	setResearchManageButtonsVisible(false);
	setSearchVisible(false);
}

void ResearchView::editResearchRoot()
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->researchRootEdit);

	//
	// Но кнопку удаления всё-равно скрываем
	//
	setResearchManageButtonsVisible(true);
	m_ui->removeResearchItem->hide();
	setSearchVisible(false);
}

void ResearchView::editText(const QString& _name, const QString& _description)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->textDataEdit);
	m_ui->textName->setText(_name);
	m_ui->textDescription->setHtml(_description);

	setResearchManageButtonsVisible(true);
	setSearchVisible(true);
}

void ResearchView::editUrl(const QString& _name, const QString& _url, const QString& _cachedContent)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->urlEdit);
	m_ui->urlName->setText(_name);
	m_ui->urlLink->setText(_url);
	if (m_ui->urlContent->url().toString() != _url) {
		m_ui->urlContent->load(QUrl(_url));
	}
	m_cachedUrlContent = _cachedContent;

	setResearchManageButtonsVisible(true);
	setSearchVisible(false);
}

void ResearchView::editImagesGallery(const QString& _name, const QList<QPixmap>& _images)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->imagesGalleryEdit);
	m_ui->imagesGalleryName->setText(_name);

	//
	// Загружаем изображения и для этого сначала отключаем уведомления о изменении галереи,
	// а после того, как всё загрузим, включаем вновь
	//
	disconnect(m_ui->imagesGalleryPane, &ImagesPane::imageAdded, this, &ResearchView::imagesGalleryImageAdded);
	disconnect(m_ui->imagesGalleryPane, &ImagesPane::imageRemoved, this, &ResearchView::imagesGalleryImageRemoved);
	m_ui->imagesGalleryPane->clear();
	foreach (const QPixmap& image, _images) {
		m_ui->imagesGalleryPane->addImage(image);
	}
	connect(m_ui->imagesGalleryPane, &ImagesPane::imageAdded, this, &ResearchView::imagesGalleryImageAdded);
	connect(m_ui->imagesGalleryPane, &ImagesPane::imageRemoved, this, &ResearchView::imagesGalleryImageRemoved);

	setResearchManageButtonsVisible(true);
	setSearchVisible(false);
}

void ResearchView::editImage(const QString& _name, const QPixmap& _image)
{
	m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->imageEdit);
	m_ui->imageName->setText(_name);
	m_ui->imagePreview->setImage(_image);

	setResearchManageButtonsVisible(true);
	setSearchVisible(false);
}

void ResearchView::setCommentOnly(bool _isCommentOnly)
{
	m_ui->addResearchItem->setEnabled(!_isCommentOnly);
	m_ui->removeResearchItem->setEnabled(!_isCommentOnly);
	m_ui->scenarioName->setReadOnly(_isCommentOnly);
	m_ui->scenarioLogline->setReadOnly(_isCommentOnly);
	m_ui->titlePageName->setReadOnly(_isCommentOnly);
	m_ui->titlePageAdditionalInfo->lineEdit()->setReadOnly(_isCommentOnly);
	m_ui->titlePageAuthor->setReadOnly(_isCommentOnly);
	m_ui->titlePageContacts->setReadOnly(_isCommentOnly);
	m_ui->titlePageGenre->setReadOnly(_isCommentOnly);
	m_ui->titlePageYear->setReadOnly(_isCommentOnly);
	m_ui->synopsisText->setReadOnly(_isCommentOnly);
	m_ui->textName->setReadOnly(_isCommentOnly);
	m_ui->textDescription->setReadOnly(_isCommentOnly);
}

bool ResearchView::eventFilter(QObject* _object, QEvent* _event)
{
	if (_object == m_ui->researchNavigator
		&& _event->type() == QEvent::ContextMenu) {
		QContextMenuEvent* contextMenuEvent = dynamic_cast<QContextMenuEvent*>(_event);
		emit navigatorContextMenuRequested(
			currentResearchIndex(), m_ui->researchNavigator->mapToGlobal(contextMenuEvent->pos()));
	}

	return QWidget::eventFilter(_object, _event);
}

void ResearchView::setResearchManageButtonsVisible(bool _isVisible)
{
	m_ui->addResearchItem->setVisible(_isVisible);
	m_ui->removeResearchItem->setVisible(_isVisible);
}

void ResearchView::setSearchVisible(bool _isVisible)
{
	m_ui->search->setVisible(_isVisible);
	m_ui->searchWidget->setVisible(m_ui->search->isVisible() && m_ui->search->isChecked());
}

void ResearchView::currentResearchChanged()
{
	QModelIndex selectedResearchIndex = currentResearchIndex();

	//
	// Испускаем сигнал о намерении изменить элемент разработки, только если он выделен
	//
	if (selectedResearchIndex.isValid()) {
		emit editResearchRequested(selectedResearchIndex);
	}
}

void ResearchView::initView()
{
	m_ui->researchNavigator->setItemDelegate(new ResearchNavigatorItemDelegate(m_ui->researchNavigator));
	m_ui->researchNavigator->setDragDropMode(QAbstractItemView::DragDrop);
	m_ui->researchNavigator->setDragEnabled(true);
	m_ui->researchNavigator->setDropIndicatorShown(true);
	m_ui->researchNavigator->setAlternatingRowColors(true);
	m_ui->researchNavigator->setHeaderHidden(true);
	m_ui->researchNavigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_ui->researchNavigator->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ui->researchNavigator->setStyle(new ResearchNavigatorProxyStyle(m_ui->researchNavigator->style()));
	m_ui->researchNavigator->installEventFilter(this);

	m_ui->researchSplitter->setObjectName("researchSplitter");
	m_ui->researchSplitter->setHandleWidth(1);
	m_ui->researchSplitter->setOpaqueResize(false);
	m_ui->researchSplitter->setStretchFactor(1, 1);

	m_ui->search->setIcons(m_ui->search->icon());

	m_ui->imagesGalleryPane->setLastSelectedImagePath(::imagesFolderPath());

	m_ui->imagePreview->setReadOnly(true);

	m_ui->searchWidget->setEditor(m_ui->textDescription);
	m_ui->searchWidget->hide();
}

void ResearchView::initConnections()
{
	//
	// Реакции на нажатие кнопок
	//
	connect(m_ui->addResearchItem, &FlatButton::clicked, [=] {
		emit addResearchRequested(currentResearchIndex());
	});
	connect(m_ui->removeResearchItem, &FlatButton::clicked, [=] {
		emit removeResearchRequested(currentResearchIndex());
	});
	connect(m_ui->search, &FlatButton::toggled, m_ui->searchWidget, &SearchWidget::setVisible);

	//
	// Внутренние соединения формы
	//
	connect(m_ui->scenarioName, &QLineEdit::textChanged, m_ui->titlePageName, &QLineEdit::setText);
	connect(m_ui->scenarioLogline, &SimpleTextEditor::textChanged, [=] {
		const QString textToSplit = m_ui->scenarioLogline->toPlainText().simplified();
		const int wordsCount = textToSplit.split(QRegExp("([^\\w,^\\\\]|(?=\\\\))+"), QString::SkipEmptyParts).size();
		m_ui->scenarioLoglineWords->setText(QString::number(wordsCount));
	});
	connect(m_ui->titlePageName, &QLineEdit::textChanged, m_ui->scenarioName, &QLineEdit::setText);
	//
	// ... загрузка ссылки
	//
	auto loadUrlFunction = [=]{
		QUrl url(m_ui->urlLink->text());
		if (url.scheme().isEmpty()) {
			url = QUrl("http://" + m_ui->urlLink->text());
			m_ui->urlLink->setText(url.toString());
			emit urlLinkChanged(url.toString());
		}
		m_ui->urlContent->load(url);
	};
	connect(m_ui->urlLink, &QLineEdit::returnPressed, loadUrlFunction);
	connect(m_ui->urlLoad, &QPushButton::clicked, loadUrlFunction);
	connect(m_ui->urlContent, &QWebEngineView::loadProgress, m_ui->urlLoadProgress, &QProgressBar::setValue);
	//
	// ... смена изображения
	//
	connect(m_ui->imageChange, &QPushButton::clicked, [=]{
		QString imagePath =
				QFileDialog::getOpenFileName(
					this,
					tr("Choose image"),
					::imagesFolderPath(),
					tr("Images (*.png *.jpeg *.jpg *.bmp *.tiff *.tif *.gif)"));
		if (!imagePath.isEmpty()) {
			::saveImagesFolderPath(imagePath);
			m_ui->imagesGalleryPane->setLastSelectedImagePath(imagePath);

			QPixmap newImage(imagePath);
			m_ui->imagePreview->setImage(newImage);
			emit imagePreviewChanged(newImage);
		}
	});


	//
	// Сигналы об изменении данных
	//
	// ... сценарий
	//
	connect(m_ui->scenarioName, &QLineEdit::textChanged, this, &ResearchView::scenarioNameChanged);
	connect(m_ui->scenarioLogline, &SimpleTextEditor::textChanged, [=]{
		emit scenarioLoglineChanged(TextEditHelper::removeDocumentTags(m_ui->scenarioLogline->toHtml()));
	});
	//
	// ... титульная страница
	//
	connect(m_ui->titlePageName, &QLineEdit::textChanged, this, &ResearchView::scenarioNameChanged);
	connect(m_ui->titlePageAdditionalInfo, &QComboBox::editTextChanged, this, &ResearchView::titlePageAdditionalInfoChanged);
	connect(m_ui->titlePageGenre, &QLineEdit::textChanged, this, &ResearchView::titlePageGenreChanged);
	connect(m_ui->titlePageAuthor, &QLineEdit::textChanged, this, &ResearchView::titlePageAuthorChanged);
	connect(m_ui->titlePageContacts, &QPlainTextEdit::textChanged, [=]{
		emit titlePageContactsChanged(m_ui->titlePageContacts->toPlainText());
	});
	connect(m_ui->titlePageYear, &QLineEdit::textChanged, this, &ResearchView::titlePageYearChanged);
	//
	// ... синопсис
	//
	connect(m_ui->synopsisText, &SimpleTextEditor::textChanged, [=]{
		emit synopsisTextChanged(TextEditHelper::removeDocumentTags(m_ui->synopsisText->toHtml()));
	});
	//
	// ... текстовый элемент
	//
	connect(m_ui->textName, &QLineEdit::textChanged, this, &ResearchView::textNameChanged);
	connect(m_ui->textDescription, &SimpleTextEditor::textChanged, [=] {
		emit textDescriptionChanged(TextEditHelper::removeDocumentTags(m_ui->textDescription->toHtml()));
	});
	//
	// ... интернет-страница
	//
	connect(m_ui->urlName, &QLineEdit::textChanged, this, &ResearchView::urlNameChanged);
	connect(m_ui->urlLink, &QLineEdit::textEdited, this, &ResearchView::urlLinkChanged);
	connect(m_ui->urlContent, &QWebEngineView::loadFinished, [=](bool _success) {
		//
		// Если страница загрузилась успешно, кэшируем её содержимое
		//
		if (_success) {
			m_ui->urlContent->page()->toHtml([=](const QString& _html){
				emit urlContentChanged(_html);
			});
		}
		//
		// Если не удалось загрузить, загружаем информацию из кэша
		//
		else {
			m_ui->urlContent->page()->setHtml(m_cachedUrlContent);
		}
	});
	//
	// ... галерея изображений
	//
	//....... уведомления об изменении самой галереи, настраиваются в методе editImagesGallery
	//
	connect(m_ui->imagesGalleryName, &QLineEdit::textChanged, this, &ResearchView::imagesGalleryNameChanged);
	connect(m_ui->imagesGalleryPane, &ImagesPane::imageAdded, [=]{
		::saveImagesFolderPath(m_ui->imagesGalleryPane->lastSelectedImagePath());
	});
	//
	// ... изображение
	//
	connect(m_ui->imageName, &QLineEdit::textChanged, this, &ResearchView::imageNameChanged);
}

void ResearchView::initStyleSheet()
{
	m_ui->topNavigatorLabel->setProperty("inTopPanel", true);
	m_ui->topNavigatorLabel->setProperty("topPanelTopBordered", true);
	m_ui->topNavigatorEndLabel->setProperty("inTopPanel", true);
	m_ui->topNavigatorEndLabel->setProperty("topPanelTopBordered", true);
	m_ui->topDataLabel->setProperty("inTopPanel", true);
	m_ui->topDataLabel->setProperty("topPanelTopBordered", true);

	m_ui->addResearchItem->setProperty("inTopPanel", true);
	m_ui->removeResearchItem->setProperty("inTopPanel", true);

	m_ui->search->setProperty("inTopPanel", true);

	m_ui->researchNavigator->setProperty("mainContainer", true);
	m_ui->researchDataEditsContainer->setProperty("mainContainer", true);
}
