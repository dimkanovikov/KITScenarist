#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../flow/flowtext.h"
#include "../shape/card.h"
#include "../scene/customgraphicsscene.h"
#include "../flow/arrowflow.h"
#include "../shape/shape.h"
#include "../xml/save_xml.h"
#include "../xml/load_xml.h"
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QClipboard>
#include <QMimeData>

QSettings *settings;

void file_save_pic (const QString &filename, QGraphicsView *view, bool isdraft)
{
	const int border = 25;
	QRectF rect = view->scene()->itemsBoundingRect();
	QRect prect = rect.adjusted(-border,-border,border,border).toRect();
	QPixmap pixmap(prect.size());
	pixmap.fill(Qt::white);
	QPainter paintr(&pixmap);
	paintr.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	view->scene()->setSelectionArea(QPainterPath());
	view->scene()->render(
			&paintr,
			pixmap.rect(),
			rect.toRect().adjusted(-border,-border,border,border)
	);
	if (isdraft)
	{
		paintr.setFont(QFont("Verdana", 20, QFont::Bold, true));
		paintr.drawText(border+2,border+2,QObject::tr("DRAFT"));
	}
	paintr.end();
	pixmap.save(filename, "PNG");
}

void MainWindow::setFilename (const QString &newname)
{
	_filename = newname;
	this->setWindowTitle("ActivityEdit" + QString(_filename.isEmpty()?"":" - ") + _filename);
}

void MainWindow::setModified(bool yes)
{
	_file_modified = yes;
	ui->lblSaved->setText(yes? tr("Modified") : tr("Saved."));
}

bool MainWindow::modified() const
{
	return _file_modified;
}

void MainWindow::setActionIcons()
{
	ui->actionNew->setIcon(QIcon::fromTheme("document-new", ui->actionNew->icon()));
	ui->actionOpen->setIcon(QIcon::fromTheme("document-open", ui->actionOpen->icon()));
	ui->actionSave->setIcon(QIcon::fromTheme("document-save", ui->actionSave->icon()));
	ui->actionSave_as->setIcon(QIcon::fromTheme("document-saveas", ui->actionSave_as->icon()));
	ui->action_Properties->setIcon(QIcon::fromTheme("document-properties", ui->action_Properties->icon()));
	ui->action_Delete->setIcon(QIcon::fromTheme("edit-clear", ui->action_Delete->icon()));
	ui->actDraft->setIcon(QIcon::fromTheme("document-edit", ui->actDraft->icon()));
	ui->action_About->setIcon(QIcon::fromTheme("help-about", ui->action_About->icon()));

	ui->actionCopy->setIcon(QIcon::fromTheme("edit-copy", ui->actionCopy->icon()));
	ui->actionCut->setIcon(QIcon::fromTheme("edit-cut", ui->actionCut->icon()));
	ui->actionPaste->setIcon(QIcon::fromTheme("edit-paste", ui->actionPaste->icon()));
	ui->actionUndo->setIcon(QIcon::fromTheme("edit-undo", ui->actionUndo->icon()));
	ui->actionRedo->setIcon(QIcon::fromTheme("edit-redo", ui->actionRedo->icon()));
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	settings = new QSettings(QApplication::applicationDirPath()+"/activityedit.ini", QSettings::IniFormat);
	ui->setupUi(this);
	setActionIcons();
	startTimer(250);

	statusBar()->addPermanentWidget(ui->lblSaved, 10000);
	statusBar()->addPermanentWidget(ui->fraZoom);
	makeToolboxGroup();
	setCentralWidget(ui->graphicsView);
	this->restoreState(qvariant_cast<QByteArray>(settings->value("MainWindowState", this->saveState())));
	this->restoreGeometry(qvariant_cast<QByteArray>(settings->value("MainWindowGeometry", this->saveGeometry())));
	ui->hslZoom->setValue(qvariant_cast<int>(settings->value("InitialZoomPos", 2)));

	ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	ui->graphicsView->setRubberBandSelectionMode(Qt::ContainsItemShape);

	ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	ui->graphicsView->setScene(new CustomGraphicsScene());
	CustomGraphicsScene *scene = dynamic_cast<CustomGraphicsScene *>(ui->graphicsView->scene());
	connect(scene, SIGNAL(stateChangedByUser()), this, SLOT(sceneStateChanged()));

	connect(ui->actionSelect, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_card, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_scene_group, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_folder, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_vline, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_hline, SIGNAL(triggered()), this, SLOT(clickedTool()));
	connect(ui->action_note, SIGNAL(triggered()), this, SLOT(clickedTool()));

	createNewDiagram();
	/*
	Shape *sh1, *sh2;
	scene->addItem(sh1=new ActionShape());
	sh1->setPos(20,80);
	scene->addItem(sh2=new ActionShape());
	Flow *f;
	scene->addItem(f=new ArrowFlow(sh1, sh2));
	scene->addItem(sh1=new ActionShape());
	sh1->setPos(100,-50);
	scene->addItem(new NoteFlow(f, sh1));
	scene->addItem(new NoteShape());
	*/
	//temp = new CustomGraphicsScene();
}

void MainWindow::deleteSelectedItems()
{
	CustomGraphicsScene *scene = static_cast<CustomGraphicsScene *>(ui->graphicsView->scene());
	scene->notifyStateChangeByUser();
	scene->removeSelectedShapes();
}

void MainWindow::makeToolboxGroup()
{
	toolboxGroup = new QActionGroup(this);
	toolboxGroup->addAction(ui->action_card);
	toolboxGroup->addAction(ui->action_hline);
	toolboxGroup->addAction(ui->action_vline);
	toolboxGroup->addAction(ui->action_note);
}

MainWindow::~MainWindow()
{
	settings->setValue("MainWindowState", this->saveState());
	settings->setValue("MainWindowGeometry", this->saveGeometry());
	settings->setValue("InitialZoomPos", ui->hslZoom->value());
	delete settings;
	delete ui;
}

void MainWindow::on_action_Delete_triggered()
{
	// мне не хочется connect руками прописывать
	deleteSelectedItems();
}

void MainWindow::timerEvent (QTimerEvent *e)
{
	checkControls();
}

void MainWindow::checkControls()
{
	CustomGraphicsScene *s = (CustomGraphicsScene *)ui->graphicsView->scene();
	ui->actionSave->setEnabled(modified());
	ui->actionUndo->setEnabled(undostack.canUndo());
	ui->actionRedo->setEnabled(undostack.canRedo());
	ui->actionCopy->setEnabled(s->selectedShapes().count());
	ui->actionCut->setEnabled(ui->actionCopy->isEnabled());
	ui->action_Properties->setEnabled(s->selectedShapes().count()==1);
	ui->action_Delete->setEnabled(ui->actionCopy->isEnabled());
	ui->actionPaste->setEnabled(QApplication::clipboard()->mimeData()->hasFormat("activitydiagram"));
}

void MainWindow::createNewDiagram()
{
	/*
	 * Т.к. SizeAnchors - это тоже элементы сцены, то при простом
	 * удалении они будут удаляться дважды - в этой функции и в ~Shape::Shape().
	 * Так что используем deleteSelectedItems, предварительно выделив все элементы.
	 */
	/*
	QList<QGraphicsItem *> items = ui->graphicsView->scene()->items();
	foreach(QGraphicsItem *item, items)
		if (item->flags() & QGraphicsItem::ItemIsSelectable)
			if (item->parentItem()==NULL)
				if (dynamic_cast<FlowText *>(item))
					qDebug() << "ERROR!";
				else
					item->setSelected(true);
	deleteSelectedItems();
	*/
	((CustomGraphicsScene *)ui->graphicsView->scene())->removeAllShapes();
	undostack.clear();
	setFilename(QString());
	setModified(false);
}

void MainWindow::on_actionNew_triggered()
{
	if (canClose())
		createNewDiagram();
}

void MainWindow::clickedTool()
{
	CustomGraphicsScene *scene = (CustomGraphicsScene *)ui->graphicsView->scene();
	ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
	if (sender()==ui->action_card) {
		scene->appendCard(CardShape::TypeScene, "Title", "Description");
	} else if (sender()==ui->action_scene_group) {
		scene->appendCard(CardShape::TypeScenesGroup, "Scene Group", "Description");
	} else if (sender()==ui->action_folder) {
		scene->appendCard(CardShape::TypeFolder, "Folder", "Description");
	} else if (sender()==ui->action_note) {
		scene->appendNote("Note text");
	} else if (sender()==ui->action_hline) {
		scene->appendHorizontalLine();
	} else if (sender()==ui->action_vline) {
		scene->appendVerticalLine();
	}
}

bool MainWindow::saveFile()
{
	if (_filename.isEmpty()) return saveFileAs();

	try
	{
		if (_fileispng) {
			file_save_pic(_filename, ui->graphicsView, false);
		} else {
			fileSaveXml(_filename, ui->graphicsView);
		}
		undostack.clear();
		setModified(false);
	}

	catch(...)
	{
		QMessageBox::critical(this, tr("ERROR!"), tr("Can't save to this file:\n\n") + _filename + tr("\n\nTry another file."));
		createNewDiagram();
	}

	return true;
}

bool MainWindow::saveFileAs()
{
	QString filename = QFileDialog::getSaveFileName(
			this,
			tr("Save as..."),
			QString(),
			tr("ActivityEdit's XML (*.aex);;Image (*.png)")
	);
	if (!filename.isEmpty())
	{
		setFilename(filename);
		_fileispng = filename.toLower().endsWith(".png");
		return saveFile();
	}
	return false;
}

bool MainWindow::canClose()
{
	if (modified())
	{
		QMessageBox msgBox;
		msgBox.setText(tr("This document has been modified."));
		msgBox.setInformativeText(tr("Do you want to save these changes?"));
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		msgBox.setIcon(QMessageBox::Warning);
		switch(msgBox.exec())
		{
			case QMessageBox::Save:
				if (!saveFile())
					return false;
				break;
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Discard:
				// nothing to do
				break;
		}
	}
	return true;
}

void MainWindow::closeEvent (QCloseEvent *e)
{
	if (canClose())
		e->accept();
	else
		e->ignore();
}

void MainWindow::on_actionQuit_triggered()
{
	close();
}


void MainWindow::on_actionSave_triggered()
{
	saveFile();
}

void MainWindow::on_actionOpen_triggered()
{
	if (canClose())
	{
		QString filename = QFileDialog::getOpenFileName(
				this,
				tr("Open"),
				QString(),
				tr("ActivityEdit's XML (*.aex)")
		);
		if (!filename.isEmpty())
		{
			createNewDiagram();

			try
			{
				bool isdraft;
				fileLoadXml(filename, (CustomGraphicsScene *)(ui->graphicsView->scene()), ui->graphicsView);
				ui->actDraft->setChecked(isdraft);
				setModified(false);
				_fileispng = false;
				setFilename(filename);
			}

			catch (FileErrorException &e)
			{
				QMessageBox::critical(this, tr("ERROR!"), tr("Can't open file:\n\n") + e.filename() + tr("\n\nYou can work only with files created in this editor."));
				createNewDiagram();
			}

			catch (...)
			{
				QMessageBox::critical(this, tr("ERROR!"), tr("Unexpected error.\nYou seem to be unauthorized to read it, or it is just a hardware/software problem."));
				createNewDiagram();
			}
		}
	}
}

void MainWindow::on_action_Properties_triggered()
{
	CustomGraphicsScene *s = (CustomGraphicsScene *)ui->graphicsView->scene();
	Shape *sh;
	if (s->selectedShapes().count()==1)
		if (sh=dynamic_cast<Shape *>(s->selectedShapes()[0])) sh->editProperties();
}

void MainWindow::on_actionSave_as_triggered()
{
	saveFileAs();
}

void MainWindow::on_hslZoom_valueChanged(int value)
{
	QTransform transform;
	int percent = ui->hslZoom->value() * 25 + 50;
	double scale = percent/100.0;
	transform.scale(scale, scale);
	ui->lblZoom->setText(QString::number(percent)+"%");
	ui->graphicsView->setTransform(transform);
}

void MainWindow::on_action_3_triggered()
{
	//ui->graphicsView->setScene(temp);
}

void MainWindow::sceneStateChanged()
{
	CustomGraphicsScene *scene = dynamic_cast<CustomGraphicsScene *>(ui->graphicsView->scene());
	undostack.addState(scene->toXML());
	setModified(true);
}

void MainWindow::on_actionUndo_triggered()
{
	if (undostack.canUndo())
	{
		CustomGraphicsScene *scene = dynamic_cast<CustomGraphicsScene *>(ui->graphicsView->scene());
		if (!undostack.canRedo())
		{
			undostack.addState(scene->toXML());
			undostack.undo();
		}
		scene->fromXML(undostack.undo(), scene);
		setModified(undostack.canUndo());
	}
}

void MainWindow::on_actionRedo_triggered()
{
	if (undostack.canRedo())
	{
		CustomGraphicsScene *scene = dynamic_cast<CustomGraphicsScene *>(ui->graphicsView->scene());
		scene->fromXML(undostack.redo(), scene);
		setModified(true);
	}
}

void MainWindow::on_actionSelectAll_triggered()
{
	QPainterPath path;
	path.addRect(ui->graphicsView->scene()->sceneRect());
	ui->graphicsView->scene()->setSelectionArea(path);
	ui->graphicsView->scene()->setSelectionArea(path);
}
