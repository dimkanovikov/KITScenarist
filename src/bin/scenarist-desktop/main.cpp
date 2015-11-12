#include "Application.h"

#include <ManagementLayer/ApplicationManager.h>
#include <QtWidgets>

namespace {
	QGraphicsOpacityEffect* createEffect(QWidget* _parent) {
		QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(_parent);
		effect->setOpacity(0.2);
		return effect;
	}
}

int main(int argc, char *argv[])
{
	Application application(argc, argv);

//	QWidget* w = new QWidget;
//	QLabel* lbl1 = new QLabel("Without opacity", w);
//	QLabel* lbl2 = new QLabel("With opacity", w);
//	QTreeView* tree1 = new QTreeView(w);
//	tree1->setModel(new QDirModel(tree1));
//	tree1->expand(tree1->model()->index(0,0));
//	QTreeView* tree2 = new QTreeView(w);
//	tree2->setModel(new QDirModel(tree2));
//	tree2->expand(tree2->model()->index(0,0));
//	QPushButton* btn1 = new QPushButton("Without opacity", w);
//	QPushButton* btn2 = new QPushButton("With opacity", w);
//	QGridLayout* layout = new QGridLayout(w);
//	layout->addWidget(lbl1,0,0);
//	layout->addWidget(tree1,1,0);
//	layout->addWidget(btn1,2,0);
//	layout->addWidget(lbl2,0,1);
//	layout->addWidget(tree2,1,1);
//	layout->addWidget(btn2,2,1);

////	lbl2->setGraphicsEffect(::createEffect(lbl2));
////	tree2->setGraphicsEffect(::createEffect(tree2));
////	tree2->viewport()->setGraphicsEffect(::createEffect(tree2->viewport()));
////	btn2->setGraphicsEffect(::createEffect(btn2));
//	QGraphicsOpacityEffect* effect = ::createEffect(tree2);
//	tree2->viewport()->setGraphicsEffect(effect);

//	w->show();

//	QPropertyAnimation* anim = new QPropertyAnimation(tree2);
//	anim->setTargetObject(effect);
//	anim->setPropertyName("opacity");
//	anim->setDuration(3000);
//	anim->setStartValue(effect->opacity());
//	anim->setEndValue(1);
//	anim->setEasingCurve(QEasingCurve::Linear);
//	anim->start(QAbstractAnimation::DeleteWhenStopped);

	//
	// Получим имя файла, который пользователь возможно хочет открыть
	//
	QString fileToOpen = application.arguments().value(1, QString::null);
	ManagementLayer::ApplicationManager applicationManager;
	applicationManager.exec(fileToOpen);

	//
	// Установим управляющего в приложение, для возможности открытия файлов
	//
	application.setupManager(&applicationManager);

	return application.exec();
}
