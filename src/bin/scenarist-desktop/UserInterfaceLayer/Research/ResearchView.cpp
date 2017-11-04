#include "ResearchView.h"
#include "ui_ResearchView.h"

#include <Domain/Research.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Research/ResearchModel.h>

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>

#include <QDomDocument>
#include <QFileDialog>
#include <QScrollBar>
#include <QShortcut>
#include <QStandardPaths>
#include <QStringListModel>
#include <QTimer>
#include <QXmlStreamReader>

using UserInterface::ResearchView;

namespace {
    /**
     * @brief Ключ для доступа к папке с загружаемыми картинками
     */
    const QString IMAGES_FOLDER_KEY = "research/images-folder";

    /**
     * @brief Ключ для доступа к папке с сохраняемыми картинками ментальных карт
     */
    const QString MINDMAPS_FOLDER_KEY = "research/mindmaps-folder";

    /**
     * @brief Обновить текст в редакторе
     */
    static void updateText(QWidget* _edit, const QString& _text) {
        if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(_edit)) {
            if (lineEdit->text() != _text) {
                lineEdit->setText(_text);
            }
        } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(_edit)) {
            if (comboBox->currentText() != _text) {
                comboBox->setEditText(_text);
            }
        } else if (QTextEdit* textEdit = qobject_cast<QTextEdit*>(_edit)) {
            if (textEdit->toPlainText() != _text) {
                textEdit->setPlainText(_text);
            }
        } else if (SimpleTextEditorWidget* simpleTextEdit = qobject_cast<SimpleTextEditorWidget*>(_edit)) {
            simpleTextEdit->setHtml(_text);
        }
    }
}


ResearchView::ResearchView(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::ResearchView),
    m_isInTextFormatUpdate(false)
{
    m_ui->setupUi(this);

    initView();
    initConnections();
    initStyleSheet();
    initIconsColor();
}

ResearchView::~ResearchView()
{
    delete m_ui;
}

void ResearchView::setTextSettings(QPageSize::PageSizeId _pageSize, const QMarginsF& _margins, Qt::Alignment _numberingAlign, const QFont& _font)
{
    //
    // Настроим размер страницы для синопсиса, говорят это важно
    //
    m_ui->synopsisText->setPageSettings(_pageSize, _margins, _numberingAlign);

    //
    // Задаём шрифт по умолчанию для всех остальных редакторов текста
    //
    QVector<SimpleTextEditorWidget*> editors = { m_ui->scenarioLogline, m_ui->synopsisText, m_ui->textDescription, m_ui->characterDescription, m_ui->locationDescription };
    for (auto* editor : editors) {
        editor->setDefaultFont(_font);
    }
}

void ResearchView::setResearchModel(QAbstractItemModel* _model)
{
    //
    // Отключаем соединения от старой модели
    //
    if (QAbstractItemModel* oldModel = m_ui->researchNavigator->model()) {
        disconnect(m_ui->researchNavigator->selectionModel(), &QItemSelectionModel::selectionChanged,
                this, &ResearchView::currentResearchChanged);
        disconnect(oldModel);
    }

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
        //
        // Обновляет текущий редактор, если данные сменились соавтором
        //
        connect(_model, &QAbstractItemModel::dataChanged, [=] (const QModelIndex& _index) {
            if (_index == currentResearchIndex()) {
                //
                // Сохраняем позицию области прокрутки текущего редактора
                //
                QAbstractScrollArea* scrollArea = nullptr;
                if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->synopsisEdit) {
                    scrollArea = m_ui->synopsisText->editor();
                } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->characterEdit) {
                    scrollArea = m_ui->characterDescription->editor();
                } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->locationEdit) {
                    scrollArea = m_ui->locationDescription->editor();
                } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->textDataEdit) {
                    scrollArea = m_ui->textDescription->editor();
                } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->imagesGalleryEdit) {
                    scrollArea = m_ui->imagesGalleryPane;
                } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->mindMapEdit) {
                    scrollArea = m_ui->mindMap;
                }
                //
                int horizontalScroll = 0;
                int verticalScroll = 0;
                if (scrollArea != nullptr) {
                    horizontalScroll = scrollArea->horizontalScrollBar()->value();
                    verticalScroll = scrollArea->verticalScrollBar()->value();
                }

                //
                // Обновляем элемент
                //
                currentResearchChanged();

                //
                // Восстанавливаем позицию редактирования
                //
                if (scrollArea != nullptr) {
                    scrollArea->horizontalScrollBar()->setValue(horizontalScroll);
                    scrollArea->verticalScrollBar()->setValue(verticalScroll);
                }
            }
        });
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
    m_ui->researchNavigator->expand(_index.parent());
    m_ui->researchNavigator->setCurrentIndex(_index);
}

void ResearchView::editScenario(const QString& _name, const QString& _logline)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->scenarioEdit);
    ::updateText(m_ui->scenarioName, _name);
    ::updateText(m_ui->scenarioLogline, _logline);

    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
}

void ResearchView::editTitlePage(const QString& _name, const QString& _additionalInfo,
    const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->titlePageEdit);
    ::updateText(m_ui->titlePageName, _name);
    ::updateText(m_ui->titlePageAdditionalInfo, _additionalInfo);
    ::updateText(m_ui->titlePageGenre, _genre);
    ::updateText(m_ui->titlePageAuthor, _author);
    ::updateText(m_ui->titlePageContacts, _contacts);
    ::updateText(m_ui->titlePageYear, _year);

    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
}

void ResearchView::editSynopsis(const QString& _synopsis)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->synopsisEdit);
    if (TextEditHelper::removeDocumentTags(m_ui->synopsisText->toHtml()) != _synopsis) {
        m_ui->synopsisText->setHtml(_synopsis);
    }

    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
}

void ResearchView::editCharactersRoot()
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->charactersRootEdit);

    //
    // Но кнопку удаления всё-равно скрываем
    //
    setResearchManageButtonsVisible(true, false, true);
    m_ui->refreshResearchSubtree->setToolTip(tr("Find All Characters from Script"));
    setSearchVisible(false);
}

void ResearchView::editCharacter(const QString& _name, const QString& _realName, const QString& _description)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->characterEdit);
    m_ui->characterName->setAcceptedText(_name);
    m_ui->characterRealName->setText(_realName);
    m_ui->characterDescription->setHtml(_description);

    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
}

void ResearchView::editLocationsRoot()
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->locationsRootEdit);

    //
    // Но кнопку удаления всё-равно скрываем
    //
    setResearchManageButtonsVisible(true, false, true);
    m_ui->refreshResearchSubtree->setToolTip(tr("Find All Locations from Script"));
    setSearchVisible(false);
}

void ResearchView::editLocation(const QString& _name, const QString& _description)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->locationEdit);
    m_ui->locationName->setAcceptedText(_name);
    m_ui->locationDescription->setHtml(_description);

    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
}

void ResearchView::editResearchRoot()
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->researchRootEdit);

    //
    // Но кнопку удаления всё-равно скрываем
    //
    setResearchManageButtonsVisible(true, false);
    setSearchVisible(false);
}

void ResearchView::editText(const QString& _name, const QString& _description)
{
    //
    // Сохраняем позицию предыдущего текста
    //
    const QString oldText = TextEditHelper::removeDocumentTags(m_ui->textDescription->toHtml());
    m_textScrollingMap[oldText] = m_ui->textDescription->editor()->verticalScrollBar()->value();

    //
    // Загружаем новые данные
    //
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->textDataEdit);

    if (m_ui->textName->text() != _name) {
        m_ui->textName->setText(_name);
    }

    m_ui->textDescription->setHtml(_description);

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(true);
    //
    // ... восстанавливаем позицию прокрутки текста, если это возможно
    //
    if (m_textScrollingMap.contains(_description)) {
        m_ui->textDescription->editor()->verticalScrollBar()->setValue(m_textScrollingMap[_description]);
        //
        // Удаляем себя из карты, чтобы не засорять память, т.к. текст может быть изменён
        //
        m_textScrollingMap.remove(_description);
    }
}

void ResearchView::editUrl(const QString& _name, const QString& _url, const QString& _cachedContent)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->urlEdit);

    if (m_ui->urlName->text() != _name) {
        m_ui->urlName->setText(_name);
    }

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

    if (m_ui->imagesGalleryName->text() != _name) {
        m_ui->imagesGalleryName->setText(_name);
    }

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

    if (m_ui->imageName->text() != _name) {
        m_ui->imageName->setText(_name);
    }

    m_ui->imagePreview->setImage(_image);

    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
}

void ResearchView::editMindMap(const QString &_name, const QString &_xml)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->mindMapEdit);

    if (m_ui->mindMapName->text() != _name) {
        m_ui->mindMapName->setText(_name);
    }

    m_ui->mindMap->closeScene();
    if (_xml.isEmpty()) {
        m_ui->mindMap->newScene();
    } else {
        m_ui->mindMap->load(_xml);
    }

    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
}

void ResearchView::setCommentOnly(bool _isCommentOnly)
{
    m_ui->researchNavigator->setContextMenuPolicy(_isCommentOnly ? Qt::PreventContextMenu : Qt::DefaultContextMenu);
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
    m_ui->characterName->setReadOnly(_isCommentOnly);
    m_ui->characterRealName->setReadOnly(_isCommentOnly);
    m_ui->characterDescription->setReadOnly(_isCommentOnly);
    m_ui->locationName->setReadOnly(_isCommentOnly);
    m_ui->locationDescription->setReadOnly(_isCommentOnly);
    m_ui->textName->setReadOnly(_isCommentOnly);
    m_ui->textDescription->setReadOnly(_isCommentOnly);
    m_ui->mindMapName->setReadOnly(_isCommentOnly);
    m_ui->mindMapToolbar->setEnabled(!_isCommentOnly);
    m_ui->mindMap->setReadOnly(_isCommentOnly);
    m_ui->imagesGalleryName->setReadOnly(_isCommentOnly);
    m_ui->imagesGalleryPane->setReadOnly(_isCommentOnly);
    m_ui->imageName->setReadOnly(_isCommentOnly);
    m_ui->imageChange->setEnabled(!_isCommentOnly);
//    m_ui->imageEdit->setReadOnly
    m_ui->urlName->setReadOnly(_isCommentOnly);
    m_ui->urlLink->setReadOnly(_isCommentOnly);
    m_ui->addFolder->setEnabled(!_isCommentOnly);
    m_ui->addText->setEnabled(!_isCommentOnly);
    m_ui->addMindMap->setEnabled(!_isCommentOnly);
    m_ui->addImagesGallery->setEnabled(!_isCommentOnly);
    m_ui->addUrl->setEnabled(!_isCommentOnly);
    m_ui->searchWidget->setSearchOnly(_isCommentOnly);
}

QStringList ResearchView::expandedIndexes() const
{
    QStringList indexes;

    //
    // prepare list
    // PS: getPersistentIndexList() function is a simple `return this->persistentIndexList()` from TreeModel model class
    //
    if (BusinessLogic::ResearchModel* model = qobject_cast<BusinessLogic::ResearchModel*>(m_ui->researchNavigator->model())) {
        for (const QModelIndex& index : model->getPersistentIndexList()) {
            if (m_ui->researchNavigator->isExpanded(index)) {
                indexes << QString("%1;%2;%3").arg(index.row()).arg(index.column()).arg(index.data(Qt::DisplayRole).toString());
            }
        }
    }

    return indexes;
}

void ResearchView::setExpandedIndexes(const QStringList& _indexes)
{
    QAbstractItemModel* model = m_ui->researchNavigator->model();
    if (model != nullptr) {
        for (const QString& item : _indexes) {
            if (!item.isEmpty()) {
                QStringList itemData = item.split(";");
                const int row = itemData.takeFirst().toInt();
                const int column = itemData.takeFirst().toInt();
                const QString text = itemData.join(";");

                //
                // search `item` text in model
                //
                QModelIndexList items = model->match(model->index(0, 0), Qt::DisplayRole, QVariant::fromValue(text), -1, Qt::MatchRecursive);
                while (!items.isEmpty()) {
                    QModelIndex first = items.takeFirst();
                    if (first.row() == row
                        && first.column() == column) {
                        //
                        // Information: with this code, expands ONLY first level in QTreeView
                        //
                        m_ui->researchNavigator->setExpanded(first, true);
                        break;
                    }
                }
            }
        }
    }
}

bool ResearchView::event(QEvent* _event)
{
    if (_event->type() == QEvent::PaletteChange) {
        initStyleSheet();
        initIconsColor();
    }

    return QWidget::event(_event);
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

void ResearchView::setResearchManageButtonsVisible(bool _isVisible, bool _isDeleteVisible, bool _isRefreshVisible)
{
    if (_isVisible) {
        m_ui->addResearchItem->setVisible(_isVisible);
        if (!_isDeleteVisible) {
            m_ui->removeResearchItem->setVisible(_isDeleteVisible);
            m_ui->refreshResearchSubtree->setVisible(_isRefreshVisible);
        } else {
            m_ui->refreshResearchSubtree->setVisible(_isRefreshVisible);
            m_ui->removeResearchItem->setVisible(_isDeleteVisible);
        }
    } else {
        m_ui->addResearchItem->setVisible(_isVisible);
        m_ui->removeResearchItem->setVisible(_isVisible);
        m_ui->refreshResearchSubtree->setVisible(_isVisible);
    }
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

void ResearchView::saveMindMapAsImageFile()
{
    const QString saveFilePath = DataStorageLayer::StorageFacade::settingsStorage()->documentFilePath(MINDMAPS_FOLDER_KEY, m_ui->mindMapName->text());
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save mind map"), saveFilePath, tr("PNG files (*.png)"));
    if (!filePath.isEmpty()) {
        if (!filePath.endsWith(".png")) {
            filePath.append(".png");
        }

        m_ui->mindMap->saveToImageFile(filePath);

        DataStorageLayer::StorageFacade::settingsStorage()->saveDocumentFolderPath(MINDMAPS_FOLDER_KEY, filePath);
    }
}

void ResearchView::initView()
{
    m_ui->addResearchItem->setIcons(m_ui->addResearchItem->icon());
    m_ui->addResearchItem->setToolTip(ShortcutHelper::makeToolTip(m_ui->addResearchItem->toolTip(),
                                                                  QKeySequence(QKeySequence::New)));

    m_ui->removeResearchItem->setIcons(m_ui->removeResearchItem->icon());
    m_ui->refreshResearchSubtree->setIcons(m_ui->refreshResearchSubtree->icon());

    m_ui->researchNavigator->setItemDelegate(new TreeViewItemDelegate(m_ui->researchNavigator));
    m_ui->researchNavigator->setDragDropMode(QAbstractItemView::DragDrop);
    m_ui->researchNavigator->setDragEnabled(true);
    m_ui->researchNavigator->setDropIndicatorShown(true);
    m_ui->researchNavigator->setHeaderHidden(true);
    m_ui->researchNavigator->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_ui->researchNavigator->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ui->researchNavigator->setStyle(new TreeViewProxyStyle(m_ui->researchNavigator->style()));
    m_ui->researchNavigator->installEventFilter(this);

    m_ui->researchSplitter->setObjectName("researchSplitter");
    m_ui->researchSplitter->setHandleWidth(1);
    m_ui->researchSplitter->setOpaqueResize(false);
    m_ui->researchSplitter->setStretchFactor(1, 1);

    m_ui->search->setIcons(m_ui->search->icon());

    m_ui->scenarioLogline->setToolbarVisible(false);

    m_ui->synopsisText->setUsePageMode(true);

    QFont nameFont = m_ui->characterName->font();
    nameFont.setCapitalization(QFont::AllUppercase);
    m_ui->characterName->setFont(nameFont);
    m_ui->characterName->setQuestionPrefix(tr("Character name"));
    m_ui->locationName->setFont(nameFont);
    m_ui->locationName->setQuestionPrefix(tr("Location name"));

    const QString imagesFolderPath = DataStorageLayer::StorageFacade::settingsStorage()->documentFolderPath(IMAGES_FOLDER_KEY);
    m_ui->imagesGalleryPane->setLastSelectedImagePath(imagesFolderPath);

    m_ui->imagePreview->setReadOnly(true);

    m_ui->addRootNode->setColorsPane(ColoredToolButton::Google);
    m_ui->addRootNode->setColor(Node::defaultBackgroundColor);
    m_ui->addRootNode->setToolTip(ShortcutHelper::makeToolTip(m_ui->addRootNode->toolTip(),
                                                              QKeySequence(QKeySequence::New)));
    m_ui->addNode->setIcons(m_ui->addNode->icon());
    m_ui->addNode->setShortcut(QKeySequence("Ctrl+Return"));
    m_ui->addNode->setToolTip(ShortcutHelper::makeToolTip(m_ui->addNode->toolTip(),
                                                          m_ui->addNode->shortcut()));
    m_ui->addSiblingNode->setIcons(m_ui->addSiblingNode->icon());
    m_ui->addSiblingNode->setShortcut(QKeySequence("Shift+Return"));
    m_ui->addSiblingNode->setToolTip(ShortcutHelper::makeToolTip(m_ui->addSiblingNode->toolTip(),
                                                                  m_ui->addSiblingNode->shortcut()));
    m_ui->deleteNode->setIcons(m_ui->deleteNode->icon());
    m_ui->biggerNode->setIcons(m_ui->biggerNode->icon());
    m_ui->smallerNode->setIcons(m_ui->smallerNode->icon());
    m_ui->nodeTextColor->setColorsPane(ColoredToolButton::Google);
    m_ui->nodeBackgroundColor->setColorsPane(ColoredToolButton::Google);
    m_ui->addEdge->setIcons(m_ui->addEdge->icon());
    m_ui->deleteEdge->setIcons(m_ui->deleteEdge->icon());
    m_ui->mindMapSaveToFile->setIcons(m_ui->mindMapSaveToFile->icon());

    m_ui->searchWidget->setEditor(m_ui->textDescription->editor());
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
    QShortcut* addResearchShortcut = new QShortcut(QKeySequence::New, m_ui->researchNavigator);
    addResearchShortcut->setContext(Qt::WidgetShortcut);
    connect(addResearchShortcut, &QShortcut::activated, m_ui->addResearchItem, &FlatButton::click);
    //
    connect(m_ui->removeResearchItem, &FlatButton::clicked, [=] {
        emit removeResearchRequested(currentResearchIndex());
    });
    QShortcut* removeShortcut = new QShortcut(QKeySequence::Delete, m_ui->researchNavigator);
    removeShortcut->setContext(Qt::WidgetShortcut);
    connect(removeShortcut, &QShortcut::activated, m_ui->removeResearchItem, &FlatButton::click);
    QShortcut* removeShortcut2 = new QShortcut(QKeySequence("Backspace", QKeySequence::PortableText), m_ui->researchNavigator);
    removeShortcut2->setContext(Qt::WidgetShortcut);
    connect(removeShortcut2, &QShortcut::activated, m_ui->removeResearchItem, &FlatButton::click);
    //
    connect(m_ui->refreshResearchSubtree, &FlatButton::clicked, [=] {
        emit refeshResearchSubtreeRequested(currentResearchIndex());
    });
    //
    QShortcut* searchShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    searchShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(searchShortcut, &QShortcut::activated, [=] {
        //
        // Если поиск виден, но в нём нет фокуса - установим фокус в него
        // В остальных случаях просто покажем, или скроем поиск
        //
        if (m_ui->search->isChecked()
            && m_ui->searchWidget->isVisible()
            && !m_ui->searchWidget->hasFocus()) {
            m_ui->searchWidget->selectText();
            m_ui->searchWidget->setFocus();
        }
        //
        // В противном случае сменим видимость
        //
        else {
            m_ui->search->toggle();
        }
    });
    connect(m_ui->search, &FlatButton::toggled, [=] (bool _toggled) {
        const bool visible = _toggled;
        if (m_ui->searchWidget->isVisible() != visible) {
            const bool FIX = true;
            const int slideDuration = WAF::Animation::slide(m_ui->searchWidget, WAF::FromBottomToTop, FIX, !FIX, visible);
            QTimer::singleShot(slideDuration, [=] { m_ui->searchWidget->setVisible(visible); });
        }

        if (visible) {
            m_ui->searchWidget->selectText();
            m_ui->searchWidget->setFocus();
        } else {
            m_ui->textDescription->setFocus();
        }
    });
    connect(m_ui->addFolder, &QPushButton::clicked, [=] {
        emit addResearchRequested(currentResearchIndex(), Domain::Research::Folder);
    });
    connect(m_ui->addText, &QPushButton::clicked, [=] {
        emit addResearchRequested(currentResearchIndex(), Domain::Research::Text);
    });
    connect(m_ui->addMindMap, &QPushButton::clicked, [=] {
        emit addResearchRequested(currentResearchIndex(), Domain::Research::MindMap);
    });
    connect(m_ui->addImagesGallery, &QPushButton::clicked, [=] {
        emit addResearchRequested(currentResearchIndex(), Domain::Research::ImagesGallery);
    });
    connect(m_ui->addUrl, &QPushButton::clicked, [=] {
        emit addResearchRequested(currentResearchIndex(), Domain::Research::Url);
    });

    //
    // Внутренние соединения формы
    //
    connect(m_ui->scenarioName, &QLineEdit::textChanged, [=] {
        ::updateText(m_ui->titlePageName, m_ui->scenarioName->text());
    });
    connect(m_ui->scenarioLogline, &SimpleTextEditorWidget::textChanged, [=] {
        const QString textToSplit = m_ui->scenarioLogline->toPlainText().simplified();
        const int wordsCount = textToSplit.split(QRegExp("([^\\w,^\\\\]|(?=\\\\))+"), QString::SkipEmptyParts).size();
        m_ui->scenarioLoglineWords->setVisible(wordsCount > 0);
        m_ui->scenarioLoglineWordsLabel->setVisible(wordsCount > 0);
        m_ui->scenarioLoglineWords->setText(QString::number(wordsCount));
        const QString color =
                wordsCount <= 25
                ? "green"
                : wordsCount <= 30
                  ? "palette(text)"
                  : "red";
        m_ui->scenarioLoglineWords->setStyleSheet("color: " + color);
        m_ui->scenarioLoglineWordsLabel->setStyleSheet("color: " + color);
    });
    connect(m_ui->titlePageName, &QLineEdit::textChanged, [=] {
        ::updateText(m_ui->scenarioName, m_ui->titlePageName->text());
    });
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
        const QString imagesFolderPath = DataStorageLayer::StorageFacade::settingsStorage()->documentFolderPath(IMAGES_FOLDER_KEY);
        QString imagePath =
                QFileDialog::getOpenFileName(this, tr("Choose image"), imagesFolderPath,
                                             tr("Images (*.png *.jpeg *.jpg *.bmp *.tiff *.tif *.gif)"));
        if (!imagePath.isEmpty()) {
            DataStorageLayer::StorageFacade::settingsStorage()->saveDocumentFolderPath(IMAGES_FOLDER_KEY, imagePath);
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
    connect(m_ui->scenarioLogline, &SimpleTextEditorWidget::textChanged, [=]{
        emit scenarioLoglineChanged(TextEditHelper::removeDocumentTags(m_ui->scenarioLogline->toHtml()));
    });
    //
    // ... титульная страница
    //
    connect(m_ui->titlePageName, &QLineEdit::textChanged, this, &ResearchView::scenarioNameChanged);
    connect(m_ui->titlePageAdditionalInfo, &QComboBox::editTextChanged, this, &ResearchView::titlePageAdditionalInfoChanged);
    connect(m_ui->titlePageGenre, &QLineEdit::textChanged, this, &ResearchView::titlePageGenreChanged);
    connect(m_ui->titlePageAuthor, &QLineEdit::textChanged, this, &ResearchView::titlePageAuthorChanged);
    connect(m_ui->titlePageContacts, &QTextEdit::textChanged, [=]{
        emit titlePageContactsChanged(m_ui->titlePageContacts->toPlainText());
    });
    connect(m_ui->titlePageYear, &QLineEdit::textChanged, this, &ResearchView::titlePageYearChanged);
    //
    // ... синопсис
    //
    connect(m_ui->synopsisText, &SimpleTextEditorWidget::textChanged, [=]{
        emit synopsisTextChanged(TextEditHelper::removeDocumentTags(m_ui->synopsisText->toHtml()));
    });
    //
    // ... персонаж
    //
    connect(m_ui->characterName, &AcceptebleLineEdit::textAccepted, this, &ResearchView::characterNameChanged);
    connect(m_ui->characterRealName, &QLineEdit::textChanged, this, &ResearchView::characterRealNameChanged);
    connect(m_ui->characterDescription, &SimpleTextEditorWidget::textChanged, [=] {
        emit characterDescriptionChanged(m_ui->characterDescription->toHtml());
    });
    //
    // ... локация
    //
    connect(m_ui->locationName, &AcceptebleLineEdit::textAccepted, this, &ResearchView::locationNameChanged);
    connect(m_ui->locationDescription, &SimpleTextEditorWidget::textChanged, [=] {
        emit locationDescriptionChanged(TextEditHelper::removeDocumentTags(m_ui->locationDescription->toHtml()));
    });
    //
    // ... текстовый элемент
    //
    connect(m_ui->textName, &QLineEdit::textChanged, this, &ResearchView::textNameChanged);
    connect(m_ui->textDescription, &SimpleTextEditorWidget::textChanged, [=] {
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
        DataStorageLayer::StorageFacade::settingsStorage()->saveDocumentFolderPath(
                    IMAGES_FOLDER_KEY, m_ui->imagesGalleryPane->lastSelectedImagePath());
    });
    //
    // ... изображение
    //
    connect(m_ui->imageName, &QLineEdit::textChanged, this, &ResearchView::imageNameChanged);

    //
    // ... ментальная карта
    //
    connect(m_ui->mindMapName, &QLineEdit::textChanged, this, &ResearchView::mindMapNameChanged);
    connect(m_ui->mindMap, &GraphWidget::contentChanged, [=] {
        emit mindMapChanged(m_ui->mindMap->save());
    });
    //
    // ... панель инструментов редактора ментальных карт
    //
    connect(m_ui->mindMap->graphLogic(), &GraphLogic::activeNodeChanged, [=] {
        if (Node* activeNode = m_ui->mindMap->graphLogic()->activeNode()) {
            m_ui->nodeTextColor->setVisible(activeNode->isRoot());
            m_ui->nodeTextColor->updateColor(activeNode->textColor());
            m_ui->nodeBackgroundColor->updateColor(activeNode->color());
        } else {
            m_ui->nodeTextColor->show();
            m_ui->nodeTextColor->updateColor(QColor());
            m_ui->nodeBackgroundColor->updateColor(QColor());
        }
    });
    connect(m_ui->addRootNode, &ColoredToolButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::insertRootNode);
    QShortcut* addNodeShortcut = new QShortcut(QKeySequence::New, m_ui->mindMapEdit);
    addNodeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(addNodeShortcut, &QShortcut::activated, m_ui->addRootNode, &ColoredToolButton::click);
    connect(m_ui->addNode, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::insertNode);
    connect(m_ui->addSiblingNode, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::insertSiblingNode);
    connect(m_ui->deleteNode, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::removeNode);
    QShortcut* removeNodeShortcut = new QShortcut(QKeySequence::Delete, m_ui->mindMapEdit);
    removeNodeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(removeNodeShortcut, &QShortcut::activated, m_ui->deleteNode, &FlatButton::click);
    QShortcut* removeNodeShortcut2 = new QShortcut(QKeySequence("Backspace", QKeySequence::PortableText), m_ui->mindMapEdit);
    removeNodeShortcut2->setContext(Qt::WidgetWithChildrenShortcut);
    connect(removeNodeShortcut2, &QShortcut::activated, m_ui->deleteNode, &FlatButton::click);
    connect(m_ui->biggerNode, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::scaleUp);
    connect(m_ui->smallerNode, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::scaleDown);
    connect(m_ui->addEdge, &FlatButton::clicked, m_ui->mindMap->graphLogic(), static_cast<void (GraphLogic::*)()>(&GraphLogic::addEdge));
    connect(m_ui->deleteEdge, &FlatButton::clicked, m_ui->mindMap->graphLogic(), static_cast<void (GraphLogic::*)()>(&GraphLogic::removeEdge));
    connect(m_ui->nodeTextColor, &ColoredToolButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::setNodeTextColor);
    connect(m_ui->nodeBackgroundColor, &ColoredToolButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::setNodeColor);
    connect(m_ui->mindMapSaveToFile, &FlatButton::clicked, this, &ResearchView::saveMindMapAsImageFile);
}

void ResearchView::initStyleSheet()
{
    m_ui->topNavigatorLabel->setProperty("inTopPanel", true);
    m_ui->topNavigatorLabel->setProperty("topPanelTopBordered", true);
    m_ui->topDataLabel->setProperty("inTopPanel", true);
    m_ui->topDataLabel->setProperty("topPanelTopBordered", true);
    m_ui->topMindMapToolbarLabelLeft->setProperty("inTopPanel", true);
    m_ui->topMindMapToolbarLabelLeft->setProperty("topPanelLeftBordered", true);
    m_ui->topMindMapToolbarLabelRight->setProperty("inTopPanel", true);
    m_ui->topMindMapToolbarLabelRight->setProperty("topPanelTopBordered", true);
    m_ui->topMindMapToolbarLabelRight->setProperty("topPanelRightBordered", true);

    m_ui->addResearchItem->setProperty("inTopPanel", true);
    m_ui->removeResearchItem->setProperty("inTopPanel", true);
    m_ui->refreshResearchSubtree->setProperty("inTopPanel", true);

    m_ui->search->setProperty("inTopPanel", true);

    m_ui->addFolder->setProperty("leftAlignedText", true);
    m_ui->addText->setProperty("leftAlignedText", true);
    m_ui->addMindMap->setProperty("leftAlignedText", true);
    m_ui->addImagesGallery->setProperty("leftAlignedText", true);
    m_ui->addUrl->setProperty("leftAlignedText", true);

    m_ui->addRootNode->setProperty("inTopPanel", true);
    m_ui->addNode->setProperty("inTopPanel", true);
    m_ui->addSiblingNode->setProperty("inTopPanel", true);
    m_ui->deleteNode->setProperty("inTopPanel", true);
    m_ui->biggerNode->setProperty("inTopPanel", true);
    m_ui->smallerNode->setProperty("inTopPanel", true);
    m_ui->addEdge->setProperty("inTopPanel", true);
    m_ui->deleteEdge->setProperty("inTopPanel", true);
    m_ui->nodeTextColor->setProperty("inTopPanel", true);
    m_ui->nodeBackgroundColor->setProperty("inTopPanel", true);
    m_ui->mindMapSaveToFile->setProperty("inTopPanel", true);

    m_ui->researchNavigator->setProperty("mainContainer", true);
    m_ui->researchDataEditsContainer->setProperty("mainContainer", true);

    m_ui->textName->setProperty("editableLabel", true);
    m_ui->urlName->setProperty("editableLabel", true);
    m_ui->imagesGalleryName->setProperty("editableLabel", true);
    m_ui->imageName->setProperty("editableLabel", true);
    m_ui->mindMapName->setProperty("editableLabel", true);
}

void ResearchView::initIconsColor()
{
    const QSize iconSize = m_ui->addFolder->iconSize();

    QIcon folder = m_ui->addFolder->icon();
    ImageHelper::setIconColor(folder, iconSize, palette().text().color());
    m_ui->addFolder->setIcon(folder);

    QIcon text = m_ui->addText->icon();
    ImageHelper::setIconColor(text, iconSize, palette().text().color());
    m_ui->addText->setIcon(text);

    QIcon mindMap = m_ui->addMindMap->icon();
    ImageHelper::setIconColor(mindMap, iconSize, palette().text().color());
    m_ui->addMindMap->setIcon(mindMap);

    QIcon imagesGallery = m_ui->addImagesGallery->icon();
    ImageHelper::setIconColor(imagesGallery, iconSize, palette().text().color());
    m_ui->addImagesGallery->setIcon(imagesGallery);

    QIcon url = m_ui->addUrl->icon();
    ImageHelper::setIconColor(url, iconSize, palette().text().color());
    m_ui->addUrl->setIcon(url);
}
