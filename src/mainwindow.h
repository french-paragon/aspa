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
	static const char* attributionDatasIndex;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

	void newInfosText(QString info);

public slots:

	void onProjectDeletionRequested();
	void onDemandDeletionRequested();

	bool openProject();
	bool closeProject();

	bool saveProject();
	bool saveProjectAs();

	void importProjects();
	void importDemands();

	bool doAttribution();
	bool exportAttributionAsHtml();
	bool exportAttributionAsPdf();

	void quit();

protected slots:

	void forwardInfoText();
	void setTextToInfos(QString const& text);

protected:

	void configureModels();
	void configureActions();
	void configureToolBar();
	void configureOptions();

	QString getSaveFileName(QString defaultExt,
						 QString filter,
						 QString dir,
						 bool forceExtension = false);

	QString getImportFileName(QString filter,
							QString dir);

private:
    Ui::MainWindow *ui;
	ProjectListModel* _projectsModel;
	DemandListModel* _demandModel;
	AttributionModel* _attributionModel;
	QString _projectFile;
};

#endif // MAINWINDOW_H
