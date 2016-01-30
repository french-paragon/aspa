#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ProjectListModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

	void onProjectDeletionRequested();

protected:

	void configureModels();

private:
    Ui::MainWindow *ui;
	ProjectListModel* _projectsModel;
};

#endif // MAINWINDOW_H
