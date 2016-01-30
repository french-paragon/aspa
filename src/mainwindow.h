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

	static const char* projectsDatasIndex;
	static const char* demandesDatasIndex;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

	void onProjectDeletionRequested();
	void onDemandDeletionRequested();

	bool saveProjectAs();

	bool doAttribution();

protected:

	void configureModels();
	void configureActions();

private:
    Ui::MainWindow *ui;
	ProjectListModel* _projectsModel;
	DemandListModel* _demandModel;
	AttributionModel* _attributionModel;
	QString _projectFile;
};

#endif // MAINWINDOW_H
