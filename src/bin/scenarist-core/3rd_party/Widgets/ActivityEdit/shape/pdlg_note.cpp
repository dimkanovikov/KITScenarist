#include "pdlg_note.h"
#include "ui_pdlg_note.h"
#include "../scene/customgraphicsscene.h"

NoteShapePropertiesDialog::NoteShapePropertiesDialog(NoteShape *shp, QWidget *parent)
	: shape(shp)
	, QDialog(parent)
	, ui(new Ui::NoteShapePropertiesDialog)
{
	ui->setupUi(this);
	ui->txtTitle->setPlainText(shape->text());
	ui->btnCancel->setIcon(QIcon::fromTheme("dialog-cancel"));
	ui->btnOkay->setIcon(QIcon::fromTheme("dialog-ok"));
}

NoteShapePropertiesDialog::~NoteShapePropertiesDialog()
{
	delete ui;
}

void NoteShapePropertiesDialog::on_btnOkay_clicked()
{
	shape->emitStateIsAboutToBeChangedByUser();
	shape->setText(ui->txtTitle->toPlainText());
	accept();
}

void NoteShapePropertiesDialog::on_btnCancel_clicked()
{
	reject();
}
