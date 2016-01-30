#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectlistmodel.h"

#include <QTableView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	 _projectsModel = new ProjectListModel(this);

	 configureModels();
}

MainWindow::~MainWindow()
{
	delete _projectsModel;
    delete ui;
}

void MainWindow::configureModels(){
	ui->projectView->setModel(_projectsModel);
	connect(ui->addProjectButton, SIGNAL(clicked()),
			_projectsModel, SLOT(createTuple()));
	connect(ui->removeProjectButton, SIGNAL(clicked()),
			this, SLOT(onProjectDeletionRequested()));
	ui->projectView->horizontalHeader()
			->setStyleSheet("QHeaderView::section{background-color:rgb(80, 100, 25); "
							"selection-background-color: transparent;}"
							"QHeaderView::section:checked{background-color:rgb(80, 100, 25); "
							"selection-background-color: transparent;}");
	ui->projectView->verticalHeader()->hide();
	ui->projectView->horizontalHeader()->setStretchLastSection(true);
}


void MainWindow::onProjectDeletionRequested(){
	QModelIndexList selecteds = ui->projectView->selectionModel()->selectedIndexes();
	_projectsModel->removeSelectedTuples(selecteds);
}
