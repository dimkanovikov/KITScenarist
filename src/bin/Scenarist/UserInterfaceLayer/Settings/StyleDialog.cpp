#include "StyleDialog.h"
#include "ui_StyleDialog.h"

StyleDialog::StyleDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::StyleDialog)
{
	ui->setupUi(this);
}

StyleDialog::~StyleDialog()
{
	delete ui;
}
