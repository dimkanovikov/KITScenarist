#include "ResearchView.h"
#include "ui_ResearchView.h"

#include <Domain/Research.h>

#include <DataLayer/DataStorageLayer/StorageFacade.h>
#include <DataLayer/DataStorageLayer/SettingsStorage.h>

#include <BusinessLayer/Research/ResearchModel.h>

#include <UserInterfaceLayer/ScenarioTextEdit/ScenarioTextEdit.h>

#include <3rd_party/Delegates/TreeViewItemDelegate/TreeViewItemDelegate.h>
#include <3rd_party/Helpers/ImageHelper.h>
#include <3rd_party/Helpers/TextEditHelper.h>
#include <3rd_party/Helpers/ShortcutHelper.h>
#include <3rd_party/Styles/TreeViewProxyStyle/TreeViewProxyStyle.h>
#include <3rd_party/Widgets/ScalableWrapper/ScalableWrapper.h>
#include <3rd_party/Widgets/SimpleTextEditor/SimpleTextEditor.h>
#include <3rd_party/Widgets/WAF/Animation/Animation.h>
#include <3rd_party/Widgets/WAF/StackedWidgetAnimation/StackedWidgetAnimation.h>

#include <QDomDocument>
#include <QFileDialog>
#include <QScrollBar>
#include <QShortcut>
#include <QStandardPaths>
#include <QStringListModel>
#include <QTimer>
#include <QXmlStreamReader>

using UserInterface::ResearchView;
using UserInterface::ScenarioTextEdit;

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


ResearchView::ResearchView(QWidget *_parent) :
    QWidget(_parent),
    m_ui(new Ui::ResearchView),
    m_editor(new ScenarioTextEdit(this)),
    m_editorWrapper(new ScalableWrapper(m_editor, this)),
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

void ResearchView::clear()
{
    m_textDocumentsParameters.clear();
    m_ui->versions->setModel(nullptr);
}

void ResearchView::setTextSettings(QPageSize::PageSizeId _pageSize, const QMarginsF& _margins, Qt::Alignment _numberingAlign, const QFont& _font)
{
    QSignalBlocker blocker(this);

    //
    // Настроим размер страницы для синопсиса, говорят это важно
    //
    m_ui->synopsisText->setPageSettings(_pageSize, _margins, _numberingAlign);

    //
    // Задаём шрифт по умолчанию для всех остальных редакторов текста
    //
    QVector<SimpleTextEditorWidget*> editors = { m_ui->loglineText, m_ui->synopsisText, m_ui->textDescription, m_ui->characterDescription, m_ui->locationDescription };
    for (auto* editor : editors) {
        editor->setDefaultFont(_font);
    }
}

void ResearchView::setScriptShowScenesNumbers(bool _show)
{
    m_editor->setShowSceneNumbers(_show);
}

void ResearchView::setScriptShowDialoguesNumbers(bool _show)
{
    m_editor->setShowDialoguesNumbers(_show);
}

void ResearchView::setScriptTextEditColors(const QColor& _textColor, const QColor& _backgroundColor)
{
    m_editor->viewport()->setStyleSheet(QString("color: %1; background-color: %2;").arg(_textColor.name(), _backgroundColor.name()));
    m_editor->setStyleSheet(QString("#scenarioEditor { color: %1; }").arg(_textColor.name()));
}

void ResearchView::setScriptDocument(BusinessLogic::ScenarioTextDocument* _document)
{
    m_editor->setScenarioDocument(_document);
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
                currentResearchChanged(QItemSelection(), QItemSelection());

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

void ResearchView::editScript(const QString& _name, const QString& _header, const QString& _footer,
    const QString& _scenesPrefix, const QString& _startSceneNumber)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->scriptEdit);
    updateText(m_ui->scriptName, _name);
    updateText(m_ui->scriptHeader, _header);
    updateText(m_ui->scriptFooter, _footer);
    updateText(m_ui->scriptSceneNumbersPrefix, _scenesPrefix);

    //
    // Мы умные программисты считаем с 0. Но обычным пользователям надо с 1
    //
    updateText(m_ui->scriptStartNumber, QString::number(_startSceneNumber.toInt() + 1));

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editTitlePage(const QString& _name, const QString& _additionalInfo,
    const QString& _genre, const QString& _author, const QString& _contacts, const QString& _year)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->titlePageEdit);
    updateText(m_ui->titlePageName, _name);
    updateText(m_ui->titlePageAdditionalInfo, _additionalInfo);
    updateText(m_ui->titlePageGenre, _genre);
    updateText(m_ui->titlePageAuthor, _author);
    updateText(m_ui->titlePageContacts, _contacts);
    updateText(m_ui->titlePageYear, _year);

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editLogline(const QString& _logline)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->loglineEdit);
    updateTextEditorText(_logline, m_ui->loglineText->editor());

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editSynopsis(const QString& _synopsis)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->synopsisEdit);
    updateTextEditorText(_synopsis, m_ui->synopsisText->editor());

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(false);
    setSearchVisible(true, m_ui->synopsisText->editor());
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editVersions(QAbstractItemModel* _versions)
{
    m_ui->versions->setModel(_versions);
    m_ui->versionsContent->setCurrentWidget(m_ui->versionsPage);
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->versionsEdit);

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(false);
    setSearchVisible(false);
    setAddVisible(true);
    connect(m_ui->addResearchContent, &FlatButton::clicked, this, &ResearchView::addScriptVersionRequested);
    setBackVisible(false);
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
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editCharacter(const QString& _name, const QString& _realName, const QString& _description)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->characterEdit);
    m_ui->characterName->setAcceptedText(_name);
    m_ui->characterRealName->setText(_realName);
    updateTextEditorText(_description, m_ui->characterDescription->editor());

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
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
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editLocation(const QString& _name, const QString& _description)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->locationEdit);
    m_ui->locationName->setAcceptedText(_name);
    updateTextEditorText(_description, m_ui->locationDescription->editor());

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editResearchRoot()
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->researchRootEdit);

    //
    // Но кнопку удаления всё-равно скрываем
    //
    setResearchManageButtonsVisible(true, false);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editText(const QString& _name, const QString& _description)
{
    //
    // Загружаем новые данные
    //
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->textDataEdit);

    if (m_ui->textName->text() != _name) {
        m_ui->textName->setText(_name);
    }
    updateTextEditorText(_description, m_ui->textDescription->editor());

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(true, m_ui->textDescription->editor());
    setAddVisible(false);
    setBackVisible(false);
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

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
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
    for (const QPixmap& image : _images) {
        m_ui->imagesGalleryPane->addImage(image);
    }
    connect(m_ui->imagesGalleryPane, &ImagesPane::imageAdded, this, &ResearchView::imagesGalleryImageAdded);
    connect(m_ui->imagesGalleryPane, &ImagesPane::imageRemoved, this, &ResearchView::imagesGalleryImageRemoved);

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editImage(const QString& _name, const QPixmap& _image)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->imageEdit);

    if (m_ui->imageName->text() != _name) {
        m_ui->imageName->setText(_name);
    }

    m_ui->imagePreview->setImage(_image);

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::editMindMap(const QString &_name, const QString &_xml)
{
    m_ui->researchDataEditsContainer->setCurrentWidget(m_ui->mindMapEdit);

    if (m_ui->mindMapName->text() != _name) {
        m_ui->mindMapName->setText(_name);
    }

    const auto currentXml = TextEditHelper::fromHtmlEscaped(m_ui->mindMap->save());
    if (_xml != currentXml) {
        m_ui->mindMap->closeScene();
        if (_xml.isEmpty()) {
            m_ui->mindMap->newScene();
        } else {
            m_ui->mindMap->load(_xml);
        }
    }

    //
    // Настраиваем интерфейс
    //
    setResearchManageButtonsVisible(true);
    setSearchVisible(false);
    setAddVisible(false);
    setBackVisible(false);
}

void ResearchView::updateTextEditorText(const QString& _text, SimpleTextEditor* _editor)
{
    //
    // Если пришёл новый текст
    //
    const QString text = TextEditHelper::fromHtmlEscaped(
                             TextEditHelper::removeDocumentTags(
                                 _editor->toHtml()));
    if (text != _text) {
        //
        // ... установим его
        //
        _editor->setHtml(_text);
        //
        // ... и восстановим предыдущие параметры редактирования данного документа
        //
        const QModelIndex index = currentResearchIndex();
        if (index.isValid()
            && m_textDocumentsParameters.contains(index)) {
            QTextCursor cursor = _editor->textCursor();
            cursor.setPosition(m_textDocumentsParameters[index].cursorPosition);
            _editor->setTextCursor(cursor);
            _editor->verticalScrollBar()->setValue(m_textDocumentsParameters[index].verticalScroll);
        }
    }
}

void ResearchView::setCommentOnly(bool _isCommentOnly)
{
    m_ui->researchNavigator->setContextMenuPolicy(_isCommentOnly ? Qt::PreventContextMenu : Qt::DefaultContextMenu);
    m_ui->addResearchItem->setEnabled(!_isCommentOnly);
    m_ui->removeResearchItem->setEnabled(!_isCommentOnly);
    m_ui->scriptName->setReadOnly(_isCommentOnly);
    m_ui->scriptHeader->setReadOnly(_isCommentOnly);
    m_ui->scriptFooter->setReadOnly(_isCommentOnly);
    m_ui->scriptSceneNumbersPrefix->setReadOnly(_isCommentOnly);
    m_ui->titlePageName->setReadOnly(_isCommentOnly);
    m_ui->titlePageAdditionalInfo->lineEdit()->setReadOnly(_isCommentOnly);
    m_ui->titlePageAuthor->setReadOnly(_isCommentOnly);
    m_ui->titlePageContacts->setReadOnly(_isCommentOnly);
    m_ui->titlePageGenre->setReadOnly(_isCommentOnly);
    m_ui->titlePageYear->setReadOnly(_isCommentOnly);
    m_ui->loglineText->setReadOnly(_isCommentOnly);
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

void ResearchView::setScenesNumberingFixed(bool _fixed)
{
    m_ui->scriptStartNumber->setEnabled(!_fixed);
    m_ui->lockScenesNumbers->setEnabled(!_fixed);
    m_ui->relockScenesNumbers->setEnabled(_fixed);
    m_ui->unlockScenesNumbers->setEnabled(_fixed);
    initIconsColor();
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

void ResearchView::setSearchVisible(bool _isVisible, SimpleTextEditor* _editor)
{
    m_ui->search->setVisible(_isVisible);
    m_ui->searchWidget->setVisible(m_ui->search->isVisible() && m_ui->search->isChecked());
    m_ui->searchWidget->setEditor(_editor);
}

void ResearchView::setAddVisible(bool _isVisible)
{
    m_ui->addResearchContent->setVisible(_isVisible);

    if (!_isVisible) {
        m_ui->addResearchContent->disconnect();
    }
}

void ResearchView::setBackVisible(bool _isVisible)
{
    m_ui->backToResearchContent->setVisible(_isVisible);

    if (!_isVisible) {
        m_ui->backToResearchContent->disconnect();
    }
}

void ResearchView::currentResearchChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(selected)

    if (!deselected.isEmpty()) {
        const QModelIndex deselectedResearchIndex = deselected.indexes().first();
        //
        // Если текущим элементом является текстовый редактор, запомним позицию прокрутки
        //
        SimpleTextEditorWidget* editorWidget = nullptr;
        if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->synopsisEdit) {
            editorWidget = m_ui->synopsisText;
        } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->characterEdit) {
            editorWidget = m_ui->characterDescription;
        } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->locationEdit) {
            editorWidget = m_ui->locationDescription;
        } else if (m_ui->researchDataEditsContainer->currentWidget() == m_ui->textDataEdit) {
            editorWidget = m_ui->textDescription;
        }
        //
        // ... запомним параметры, если редактор нашёлся
        //
        if (editorWidget != nullptr) {
            m_textDocumentsParameters[deselectedResearchIndex] =
                { editorWidget->editor()->textCursor().position(),
                  editorWidget->editor()->verticalScrollBar()->value() };
        }
    }

    //
    // Испускаем сигнал о намерении изменить элемент разработки, только если он выделен
    //
    const QModelIndex selectedResearchIndex = currentResearchIndex();
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
    m_ui->addResearchContent->setIcons(m_ui->addResearchContent->icon());
    m_ui->backToResearchContent->setIcons(m_ui->backToResearchContent->icon());

    m_ui->loglineText->setToolbarVisible(false);

    m_ui->synopsisText->setUsePageMode(true);

    m_ui->versionsContent->addWidget(m_editorWrapper);
    m_editor->setReadOnly(true);
    m_editor->setUseSpellChecker(false);
    m_editor->setPageMargins(QMarginsF{15, 5, 12, 5});
    m_editor->setUseSpellChecker(false);

    QFont nameFont = m_ui->characterName->font();
    nameFont.setCapitalization(QFont::AllUppercase);
    m_ui->characterName->setFont(nameFont);
    m_ui->characterName->setQuestionPrefix(tr("Character name"));
    m_ui->locationName->setFont(nameFont);
    m_ui->locationName->setQuestionPrefix(tr("Location name"));

    const QString imagesFolderPath = DataStorageLayer::StorageFacade::settingsStorage()->documentFolderPath(IMAGES_FOLDER_KEY);
    m_ui->imagesGalleryPane->setLastSelectedImagePath(imagesFolderPath);

    m_ui->imagePreview->setReadOnly(true);

    m_ui->mindMapUndo->setIcons(m_ui->mindMapUndo->icon());
    m_ui->mindMapUndo->setShortcut(QKeySequence::Undo);
    m_ui->mindMapUndo->setToolTip(ShortcutHelper::makeToolTip(m_ui->mindMapUndo->toolTip(),
                                                              m_ui->mindMapUndo->shortcut()));
    m_ui->mindMapRedo->setIcons(m_ui->mindMapRedo->icon());
    m_ui->mindMapRedo->setShortcut(QKeySequence::Redo);
    m_ui->mindMapRedo->setToolTip(ShortcutHelper::makeToolTip(m_ui->mindMapRedo->toolTip(),
                                                              m_ui->mindMapRedo->shortcut()));
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

    m_ui->searchWidget->hide();

    m_ui->scriptStartNumber->setValidator(new QIntValidator(this));
}

void ResearchView::initConnections()
{
    //
    // Реакции на нажатие кнопок
    //
    connect(m_ui->lockScenesNumbers, &QPushButton::clicked, [this] {
        emit scenesNumberingLockChanged(true);
    });
    connect(m_ui->relockScenesNumbers, &QPushButton::clicked, [this] {
        emit scenesNumberingLockChanged(true);
    });
    connect(m_ui->unlockScenesNumbers, &QPushButton::clicked, [this] {
        emit scenesNumberingLockChanged(false);
    });
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
    removeShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(removeShortcut, &QShortcut::activated, m_ui->removeResearchItem, &FlatButton::click);
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
    connect(m_ui->scriptName, &QLineEdit::textChanged, this, [this] {
        updateText(m_ui->titlePageName, m_ui->scriptName->text());
    });
    connect(m_ui->titlePageName, &QLineEdit::textChanged, this, [this] {
        updateText(m_ui->scriptName, m_ui->titlePageName->text());
    });
    connect(m_ui->loglineText, &SimpleTextEditorWidget::textChanged, [=] {
        const QString textToSplit = m_ui->loglineText->toPlainText().simplified();
        const int wordsCount = textToSplit.split(QRegExp("([^\\w,^\\\\]|(?=\\\\))+"), QString::SkipEmptyParts).size();
        m_ui->scriptLoglineWords->setVisible(wordsCount > 0);
        m_ui->scriptLoglineWordsLabel->setVisible(wordsCount > 0);
        m_ui->scriptLoglineWords->setText(QString::number(wordsCount));
        const QString color =
                wordsCount <= 25
                ? "green"
                : wordsCount <= 30
                  ? "palette(text)"
                  : "red";
        m_ui->scriptLoglineWords->setStyleSheet("color: " + color);
        m_ui->scriptLoglineWordsLabel->setStyleSheet("color: " + color);
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
    connect(m_ui->scriptName, &QLineEdit::textChanged, this, &ResearchView::scriptNameChanged);
    connect(m_ui->scriptHeader, &QLineEdit::textChanged, this, &ResearchView::scriptHeaderChanged);
    connect(m_ui->scriptFooter, &QLineEdit::textChanged, this, &ResearchView::scriptFooterChanged);
    connect(m_ui->scriptSceneNumbersPrefix, &QLineEdit::textChanged, this, &ResearchView::scriptSceneNumbersPrefixChanged);

    //
    // Мы, умные программисты, считаем с 0. Но пользователи с 1
    //
    connect(m_ui->scriptStartNumber, &QLineEdit::textChanged, this, [this] (const QString& _startNumber) {
        emit scriptSceneStartNumber(QString::number(_startNumber.toInt() - 1));
    });
    //
    // ... титульная страница
    //
    connect(m_ui->titlePageName, &QLineEdit::textChanged, this, &ResearchView::scriptNameChanged);
    connect(m_ui->titlePageAdditionalInfo, &QComboBox::editTextChanged, this, &ResearchView::titlePageAdditionalInfoChanged);
    connect(m_ui->titlePageGenre, &QLineEdit::textChanged, this, &ResearchView::titlePageGenreChanged);
    connect(m_ui->titlePageAuthor, &QLineEdit::textChanged, this, &ResearchView::titlePageAuthorChanged);
    connect(m_ui->titlePageContacts, &QTextEdit::textChanged, [=]{
        emit titlePageContactsChanged(m_ui->titlePageContacts->toPlainText());
    });
    connect(m_ui->titlePageYear, &QLineEdit::textChanged, this, &ResearchView::titlePageYearChanged);
    //
    // ... логлайн
    //
    connect(m_ui->loglineText, &SimpleTextEditorWidget::textChanged, [=]{
        emit loglineTextChanged(TextEditHelper::removeDocumentTags(m_ui->loglineText->toHtml()));
    });
    //
    // ... синопсис
    //
    connect(m_ui->synopsisText, &SimpleTextEditorWidget::textChanged, [=]{
        emit synopsisTextChanged(TextEditHelper::removeDocumentTags(m_ui->synopsisText->toHtml()));
    });
    //
    // ... версии
    //
    connect(m_ui->versions, &ScriptVersionsList::removeRequested, this, &ResearchView::removeScriptVersionRequested);
    connect(m_ui->versions, &ScriptVersionsList::versionClicked, this, [this] (const QModelIndex& _versionIndex) {
        emit showScriptVersionRequested(_versionIndex);
        m_editor->relayoutDocument();

        connect(m_ui->backToResearchContent, &FlatButton::clicked, this, [this] {
            WAF::StackedWidgetAnimation::slide(m_ui->versionsContent, m_ui->versionsPage, WAF::FromLeftToRight);
            setAddVisible(true);
            setBackVisible(false);
        });
        m_ui->addResearchContent->hide();
        setBackVisible(true);
        WAF::StackedWidgetAnimation::slide(m_ui->versionsContent, m_editorWrapper, WAF::FromRightToLeft);

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
    connect(m_ui->mindMapUndo, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::undo);
    connect(m_ui->mindMapRedo, &FlatButton::clicked, m_ui->mindMap->graphLogic(), &GraphLogic::redo);
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
    m_ui->addResearchContent->setProperty("inTopPanel", true);
    m_ui->backToResearchContent->setProperty("inTopPanel", true);

    m_ui->addFolder->setProperty("leftAlignedText", true);
    m_ui->addText->setProperty("leftAlignedText", true);
    m_ui->addMindMap->setProperty("leftAlignedText", true);
    m_ui->addImagesGallery->setProperty("leftAlignedText", true);
    m_ui->addUrl->setProperty("leftAlignedText", true);

    m_ui->mindMapUndo->setProperty("inTopPanel", true);
    m_ui->mindMapRedo->setProperty("inTopPanel", true);
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
    m_ui->versions->setProperty("mainContainer", true);
    m_editorWrapper->setProperty("mainContainer", true);

    m_ui->textName->setProperty("editableLabel", true);
    m_ui->urlName->setProperty("editableLabel", true);
    m_ui->imagesGalleryName->setProperty("editableLabel", true);
    m_ui->imageName->setProperty("editableLabel", true);
    m_ui->mindMapName->setProperty("editableLabel", true);
}

namespace {
void updateButtonsIconColor(QPushButton* _button) {
    QIcon icon = _button->icon();
    ImageHelper::setIconColor(icon, _button->iconSize(), _button->palette().text().color());
    _button->setIcon(icon);
}
}

void ResearchView::initIconsColor()
{
    updateButtonsIconColor(m_ui->lockScenesNumbers);
    updateButtonsIconColor(m_ui->relockScenesNumbers);
    updateButtonsIconColor(m_ui->unlockScenesNumbers);
    updateButtonsIconColor(m_ui->addFolder);
    updateButtonsIconColor(m_ui->addText);
    updateButtonsIconColor(m_ui->addMindMap);
    updateButtonsIconColor(m_ui->addImagesGallery);
    updateButtonsIconColor(m_ui->addUrl);
}
