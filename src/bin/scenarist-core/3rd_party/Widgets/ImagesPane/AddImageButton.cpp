#include "AddImageButton.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>

namespace {
	static QString g_lastPath;
}

AddImageButton::AddImageButton(QWidget *_parent) :
	QPushButton(_parent)
{
	initView();
	initConnections();
}

void AddImageButton::choosePhoto()
{
	QStringList images =
			QFileDialog::getOpenFileNames(
				this,
				tr("Choose image"),
				g_lastPath,
				tr("Images (*.png *.jpeg *.jpg *.bmp *.tiff *.tif *.gif)"));
	if (!images.isEmpty()) {
		foreach (const QString& image, images) {
			emit fileChoosed(image);
		}

		// Запомним последний открытый путь
		g_lastPath = images.first();
	}
}

void AddImageButton::initView()
{
	setIcon(QIcon(":/Graphics/Icons/Editing/add.png"));
	setText(tr("Add image"));
}

void AddImageButton::initConnections()
{
	connect(this, SIGNAL(clicked()), this, SLOT(choosePhoto()));
}
