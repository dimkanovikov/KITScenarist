#ifndef ADDIMAGEBUTTON_H
#define ADDIMAGEBUTTON_H

#include <QPushButton>

class AddImageButton : public QPushButton
{
	Q_OBJECT

public:
	explicit AddImageButton(QWidget* _parent = 0);

	void setLastSelectedImagePath(const QString& _path);

signals:
	void fileChoosed(const QString& _fileName);

private slots:
	void choosePhoto();

private:
	void initView();
	void initConnections();

};

#endif // ADDIMAGEBUTTON_H
