#include "pdlg_action.h"
#include "ui_pdlg_action.h"
#include "../scene/customgraphicsscene.h"

ActionShapePropertiesDialog::ActionShapePropertiesDialog(CardShape *shp, QWidget *parent)
	: shape(shp)
	, QDialog(parent)
	, ui(new Ui::ActionShapePropertiesDialog)
{
	ui->setupUi(this);
	ui->linTitle->setText(shape->title());
	ui->linDescription->setText(shape->description());
	ui->btnCancel->setIcon(QIcon::fromTheme("dialog-cancel"));
	ui->btnOkay->setIcon(QIcon::fromTheme("dialog-ok"));
}

ActionShapePropertiesDialog::~ActionShapePropertiesDialog()
{
	delete ui;
}

void ActionShapePropertiesDialog::on_btnOkay_clicked()
{
	shape->emitStateIsAboutToBeChangedByUser();
	shape->setTitle(ui->linTitle->text());
	shape->setDescription(ui->linDescription->text());
	shape->adjustSize();
	accept();
}

void ActionShapePropertiesDialog::on_btnCancel_clicked()
{
	reject();
}
