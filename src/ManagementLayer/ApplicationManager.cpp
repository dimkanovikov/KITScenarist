#include "ApplicationManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>
#include <BusinessLayer/Export/PdfExporter.h>

#include <QTextEdit>
#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h>
#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
#include <QTreeView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent)
{
}

namespace {
	BusinessLogic::ScenarioDocument* document = 0;
}

void ApplicationManager::exec()
{
	document = new BusinessLogic::ScenarioDocument(this);
	ScenarioTextEdit* textEdit = new ScenarioTextEdit(0, document->document());
	QTreeView* view = new QTreeView;
	view->setItemDelegate(new ScenarioNavigatorItemDelegate(view));
	view->setModel(document->model());
	QPushButton* btn = new QPushButton("Print");
	connect(btn, SIGNAL(clicked()), this, SLOT(print()));
	QVBoxLayout* leftLayout = new QVBoxLayout;
	leftLayout->addWidget(view);
	leftLayout->addWidget(btn);

	QWidget* widget = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addLayout(leftLayout);
	layout->addWidget(textEdit);

	widget->show();
}

void ApplicationManager::print()
{
	BusinessLogic::PdfExporter exporter;
	exporter.exportTo(document->document(), "/home/dimkanovikov/1.pdf");
}
