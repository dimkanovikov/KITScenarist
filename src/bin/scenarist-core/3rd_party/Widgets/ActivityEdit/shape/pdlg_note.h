#ifndef SHAPE_NOTE_PROPERTIESDIALOG_H
#define SHAPE_NOTE_PROPERTIESDIALOG_H

#include <QDialog>
#include "note.h"

namespace Ui {
	class NoteShapePropertiesDialog;
}

class NoteShapePropertiesDialog : public QDialog
{
	Q_OBJECT
public:
	explicit NoteShapePropertiesDialog(NoteShape *shp, QWidget *parent = 0);
	~NoteShapePropertiesDialog();
private:
	NoteShape *shape;
	Ui::NoteShapePropertiesDialog *ui;
private slots:
	void on_btnCancel_clicked();
	void on_btnOkay_clicked();
};

#endif // SHAPE_NOTE_PROPERTIESDIALOG_H
