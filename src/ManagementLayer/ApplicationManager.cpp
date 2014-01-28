#include "ApplicationManager.h"

#include <BusinessLayer/ScenarioDocument/ScenarioDocument.h>

#include <QTextEdit>
#include <UserInterfaceLayer/Scenario/ScenarioTextEdit/ScenarioTextEdit.h>
#include <UserInterfaceLayer/Scenario/ScenarioNavigator/ScenarioNavigatorItemDelegate.h>
#include <QTreeView>
#include <QHBoxLayout>

ApplicationManager::ApplicationManager(QObject *parent) :
	QObject(parent)
{
}

void ApplicationManager::exec()
{
	BusinessLogic::ScenarioDocument* document = new BusinessLogic::ScenarioDocument(this);
	ScenarioTextEdit* textEdit = new ScenarioTextEdit(0, document->document());
	QTreeView* view = new QTreeView;
	view->setItemDelegate(new ScenarioNavigatorItemDelegate(view));
	view->setModel(document->model());

	QWidget* widget = new QWidget;
	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(view);
	layout->addWidget(textEdit);

	widget->show();
}
