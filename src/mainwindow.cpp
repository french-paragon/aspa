#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectlistmodel.h"
#include "demandlistmodel.h"
#include "attributionmodel.h"
#include "attributor.h"

#include <QDir>
#include <QTableView>
#include <QFileDialog>
#include <QMessageBox>

#include <QJsonDocument>


const char* MainWindow::projectsDatasIndex = "projects";
const char* MainWindow::demandesDatasIndex = "demandes";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	_projectFile = "";

	 _projectsModel = new ProjectListModel(this);
	 _demandModel = new DemandListModel(this);
	 _attributionModel = nullptr;

	 configureModels();
	 configureActions();
}

MainWindow::~MainWindow()
{
	delete _projectsModel;
	delete _demandModel;
    delete ui;
}

void MainWindow::configureModels(){

	//Project Model

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

	//Demand Model

	ui->demandView->setModel(_demandModel);
	connect(ui->addDemandButton, SIGNAL(clicked()),
			_demandModel, SLOT(createTuple()));
	connect(ui->removeDemandButton, SIGNAL(clicked()),
			this, SLOT(onDemandDeletionRequested()));
	ui->demandView->horizontalHeader()
			->setStyleSheet("QHeaderView::section{background-color:rgb(211, 128, 10); "
							"selection-background-color: transparent;}"
							"QHeaderView::section:checked{background-color:rgb(211, 128, 10); "
							"selection-background-color: transparent;}");
	ui->demandView->verticalHeader()->hide();
	ui->demandView->horizontalHeader()->setStretchLastSection(true);

	ui->attributionView->horizontalHeader()
			->setStyleSheet("QHeaderView::section{background-color:rgb(144, 50, 13); "
							"selection-background-color: transparent;}"
							"QHeaderView::section:checked{background-color:rgb(144, 50, 13); "
							"selection-background-color: transparent;}");
	ui->attributionView->verticalHeader()->hide();
	ui->attributionView->horizontalHeader()->setStretchLastSection(true);
}
void MainWindow::configureActions(){

	connect(ui->actionEnregistrer_sous, SIGNAL(triggered()),
			this, SLOT(saveProjectAs()));

	connect(ui->actionRecr_e_la_table_d_attribution, SIGNAL(triggered()),
			this, SLOT(doAttribution()));

}


void MainWindow::onProjectDeletionRequested(){
	QModelIndexList selecteds = ui->projectView->selectionModel()->selectedIndexes();
	_projectsModel->removeSelectedTuples(selecteds);
}

void MainWindow::onDemandDeletionRequested(){
	QModelIndexList selecteds = ui->demandView->selectionModel()->selectedIndexes();
	_demandModel->removeSelectedTuples(selecteds);
}


bool MainWindow::saveProjectAs(){

	QFileDialog fd(this);
	fd.setWindowTitle(tr("sauver le projet sous"));
	fd.setDefaultSuffix(".aspa");
	fd.setDirectory(QDir::homePath());
	fd.setNameFilter("projets aspa (*.aspa)");

	selection:

	int code = fd.exec();

	if(code == QDialog::Rejected){
		return false;
	}

	QString saveFileName = fd.selectedFiles().first();

	if(!saveFileName.endsWith(".aspa")){
		saveFileName += ".aspa";
	}

	if(QFile(saveFileName).exists()){
		int but = QMessageBox::question(this,
							  tr("Le fichier %1 existe déjà").arg(QUrl(saveFileName).fileName()),
							  tr("Désirez-vous vraiment l'écraser ?"),
							  QMessageBox::Yes | QMessageBox::No);

		if(but == QMessageBox::No){
			goto selection;
		}
	}

	QJsonObject projects = _projectsModel->representation();
	QJsonObject demandes = _demandModel->representation();

	QJsonObject proj;

	proj.insert(projectsDatasIndex, projects);
	proj.insert(demandesDatasIndex, demandes);

	QJsonDocument doc(proj);
	QByteArray datas = doc.toJson();

	QFile out(saveFileName);

	if(!out.open(QIODevice::WriteOnly)){
		QMessageBox::critical(this,
							  "erreur lors de l'enregistrement du projet.",
							  "Impossible d'ouvrir le fichier en écriture.");
		return false;
	}

	qint64 w_stat = out.write(datas);
	out.close();

	if(w_stat < 0){
		return false;
	}

	return true;

}

bool MainWindow::doAttribution(){

	if(_attributionModel == nullptr){//new attribution
		QVector<Attribution> attr = Attributor::directAttribution(*_projectsModel,
																  *_demandModel);

		//hungarian algorithm yet not supported
		//TODO: support it.

		if(attr == QVector<Attribution>()){
			return false;
		}

		_attributionModel = new AttributionModel(_demandModel,
												 _projectsModel,
												 attr,
												 this);

		ui->attributionView->setModel(_attributionModel);

		ui->tabWidget->setCurrentIndex(3);
	}

	return false;
}
