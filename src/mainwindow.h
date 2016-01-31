#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ProjectListModel;
class DemandListModel;
class AttributionModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

	static const char* progName;

	static const char* projectsDatasIndex;
	static const char* demandesDatasIndex;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

	void onProjectDeletionRequested();
	void onDemandDeletionRequested();

	bool openProject();

	bool saveProject();
	bool saveProjectAs();

	bool doAttribution();

	void quit();

protected:

	void configureModels();
	void configureActions();
	void configureToolBar();

private:
    Ui::MainWindow *ui;
	ProjectListModel* _projectsModel;
	DemandListModel* _demandModel;
	AttributionModel* _attributionModel;
	QString _projectFile;
};

#endif // MAINWINDOW_H
