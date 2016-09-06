//#ifndef MAINWINDOW_H
//#define MAINWINDOW_H

//#include <QMainWindow>
//#include <QActionGroup>
//#include "../scene/sceneundostack.h"

//namespace Ui {
//	class MainWindow;
//}

//class MainWindow : public QMainWindow
//{
//	Q_OBJECT

//public:
//	explicit MainWindow(QWidget *parent = 0);
//	~MainWindow();
//	void setModified (bool yes = true);
//	bool modified() const;

//private:
//	void setActionIcons();
//	void setFilename (const QString &newname);
//	bool _fileispng;
//	bool canClose();
//	bool saveFile();
//	bool saveFileAs();
//	bool _file_modified;
//	void makeToolboxGroup();
//	QActionGroup *toolboxGroup;
//	Ui::MainWindow *ui;
//	QString _filename;
//	SceneUndoStack undostack;

//protected:
//	virtual void closeEvent (QCloseEvent *);
//	virtual void timerEvent (QTimerEvent *);

//private slots:
//	void checkControls();
//	void on_actionSelectAll_triggered();
//	void on_actionRedo_triggered();
//	void on_actionUndo_triggered();
//	void sceneStateChanged();
//	void on_action_3_triggered();
//	void on_hslZoom_valueChanged(int value);
//	void on_actionSave_as_triggered();
//	void on_action_Properties_triggered();
//	void on_actionOpen_triggered();
//	void on_actionSave_triggered();
//	void on_actionQuit_triggered();
//    void clickedTool();
//	void on_actionNew_triggered();
//	void on_action_Delete_triggered();

//private slots:
//	void deleteSelectedItems();
//	void createNewDiagram();
//};

//#endif // MAINWINDOW_H
