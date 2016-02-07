#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "projectlistmodel.h"
#include "demandlistmodel.h"
#include "attributionmodel.h"
#include "attributor.h"

#include "aboutdialog.h"

#include <QDir>
#include <QTableView>
#include <QFileDialog>
#include <QMessageBox>

#include <QPrinter>
#include <QTextDocument>

#include <QJsonDocument>

#include <QDebug>

const char* MainWindow::progName = "aspa";

const char* MainWindow::projectsDatasIndex = "projects";
const char* MainWindow::demandesDatasIndex = "demandes";
const char* MainWindow::attributionDatasIndex = "attribs";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	_projectFile = "";

	 _projectsModel = new ProjectListModel(this);
	 _demandModel = new DemandListModel(this);
	 _attributionModel = new AttributionModel(_demandModel,
											  _projectsModel,
											  QVector<Attribution>(),
											  this);

	 configureModels();
	 configureActions();
	 configureToolBar();
	 configureOptions();

	 setWindowTitle(progName);
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

	//Attribution Model

	ui->attributionView->setModel(_attributionModel);
	ui->attributionView->horizontalHeader()
			->setStyleSheet("QHeaderView::section{background-color:rgb(144, 50, 13); "
							"selection-background-color: transparent;}"
							"QHeaderView::section:checked{background-color:rgb(144, 50, 13); "
							"selection-background-color: transparent;}");
	ui->attributionView->verticalHeader()->hide();
	ui->attributionView->horizontalHeader()->setStretchLastSection(true);

	// in case of a change in the project throw attribution list away.
	connect(_projectsModel, SIGNAL(changedDatas()),
			_attributionModel, SLOT(clearAttributionList()));
	connect(_demandModel, SIGNAL(changedDatas()),
			_attributionModel, SLOT(clearAttributionList()));
}
void MainWindow::configureActions(){

	//menus and toolbars

	connect(ui->actionEnregistrer_sous, SIGNAL(triggered()),
			this, SLOT(saveProjectAs()));

	connect(ui->actionOuvrir_un_projet, SIGNAL(triggered()),
			this, SLOT(openProject()));

	connect(ui->actionEnregistrer_le_projet_courant, SIGNAL(triggered()),
			this, SLOT(saveProject()));

	connect(ui->actionRecr_e_la_table_d_attribution, SIGNAL(triggered()),
			this, SLOT(doAttribution()));

	connect(ui->actionFermer_le_projet, SIGNAL(triggered()),
			this, SLOT(closeProject()));

	connect(ui->actionQuitter, SIGNAL(triggered()),
			this, SLOT(quit()));

	connect(ui->actionImporter_une_liste_de_projets, SIGNAL(triggered()),
			this, SLOT(importProjects()));
	connect(ui->actionImporter_une_liste_de_demandes, SIGNAL(triggered()),
			this, SLOT(importDemands()));

	connect(ui->action_propos, SIGNAL(triggered()),
			this, SLOT(showAboutDialog()));

	//buttons

	connect(ui->exportHtmlButton, SIGNAL(clicked()),
			this, SLOT(exportAttributionAsHtml()));

	connect(ui->exportPdfButton, SIGNAL(clicked()),
			this, SLOT(exportAttributionAsPdf()));

	connect(ui->importProjectButton, SIGNAL(clicked()),
			this, SLOT(importProjects()));
	connect(ui->ImportDemandButton, SIGNAL(clicked()),
			this, SLOT(importDemands()));
}

void MainWindow::configureToolBar(){
	ui->mainToolBar->addAction(ui->actionEnregistrer_le_projet_courant);
	ui->mainToolBar->addAction(ui->actionEnregistrer_sous);
	ui->mainToolBar->addAction(ui->actionOuvrir_un_projet);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionRecr_e_la_table_d_attribution);
}
void MainWindow::configureOptions(){

	//number of choices.
	ui->numberOfChoiceSpinBox->setValue(_demandModel->numberOfChoices());
	connect(ui->numberOfChoiceSpinBox, SIGNAL(valueChanged(int)),
			_demandModel, SLOT(setNumberOfChoices(int)));
	connect(_demandModel, SIGNAL(numberOfChoicesChanged(int)),
			ui->numberOfChoiceSpinBox, SLOT(setValue(int)));

	//attribution methode
	connect(ui->algosComboBox, SIGNAL(currentIndexChanged(int)),
			_attributionModel, SLOT(setAttributionAlgos(int)));
	connect(_attributionModel, SIGNAL(algosChanged(int)),
			ui->algosComboBox, SLOT(setCurrentIndex(int)));

	//infos.
	connect(ui->titreLineEdit, SIGNAL(textChanged(QString)),
			_attributionModel, SLOT(setTitre(QString)));
	connect(_attributionModel, SIGNAL(titleChanged(QString)),
			ui->titreLineEdit, SLOT(setText(QString)));

	connect(ui->infosTextEdit, SIGNAL(textChanged()),
			this, SLOT(forwardInfoText()));
	connect(this, SIGNAL(newInfosText(QString)),
			_attributionModel, SLOT(setInfos(QString)));
	connect(_attributionModel, SIGNAL(infosChanged(QString)),
			this, SLOT(setTextToInfos(QString)));

	_attributionModel->setTitre(ui->titreLineEdit->text());
	_attributionModel->setInfos(ui->infosTextEdit->toPlainText());

}


void MainWindow::onProjectDeletionRequested(){
	QModelIndexList selecteds = ui->projectView->selectionModel()->selectedIndexes();
	_projectsModel->removeSelectedTuples(selecteds);
}

void MainWindow::onDemandDeletionRequested(){
	QModelIndexList selecteds = ui->demandView->selectionModel()->selectedIndexes();
	_demandModel->removeSelectedTuples(selecteds);
}

bool MainWindow::openProject(){

	QFileDialog fd(this);
	fd.setWindowTitle(tr("Ouvrir un projet"));
	fd.setFileMode(QFileDialog::ExistingFile);
	//fd.setDefaultSuffix(".aspa");
	fd.setDirectory(QDir::homePath());
	fd.setNameFilter("projets aspa (*.aspa)");

	//selection:

	int code = fd.exec();

	if(code == QDialog::Rejected){
		return false;
	}

	QString openFileName = fd.selectedFiles().first();

	QFile in(openFileName);

	if(!in.open(QIODevice::ReadOnly | QIODevice::Text)){
		QMessageBox::critical(this, "Error loading project file", "Unable to open file");
		return false;
	}

	QByteArray datas = in.readAll();

	QJsonParseError errors;
	QJsonDocument doc = QJsonDocument::fromJson(datas, &errors);

	if(errors.error != QJsonParseError::NoError){
		QMessageBox::critical(this, "Error parsing project file", "Json parser indicate:\n"+errors.errorString());
		return false;
	}

	if(doc.isObject()){

		QJsonObject proj = doc.object();

		closeProject();

		if(proj.contains(projectsDatasIndex)){
			QJsonObject projects = proj.value(projectsDatasIndex).toObject();

			_projectsModel->parseJsonObject(projects);
		}

		if(proj.contains(demandesDatasIndex)){
			QJsonObject demands = proj.value(demandesDatasIndex).toObject();

			_demandModel->parseJsonObject(demands);
		}

		if(proj.contains(attributionDatasIndex)){
			QJsonObject attr = proj.value(attributionDatasIndex).toObject();

			_attributionModel->parseJsonObject(attr);
		}

		_projectFile = openFileName;
		return true;

	}

	return false;

}
bool MainWindow::closeProject(){

	_projectsModel->emptyTuples();
	_demandModel->emptyTuples();
	_attributionModel->clearAll();

	return true;
}

bool MainWindow::saveProject(){

	qDebug() << "begin project saving";

	QFile out(_projectFile);

	if(!out.exists()){
		return saveProjectAs();
	}

	QJsonObject projects = _projectsModel->representation();
	QJsonObject demandes = _demandModel->representation();
	QJsonObject attributions = _attributionModel->representation();

	QJsonObject proj;

	proj.insert(projectsDatasIndex, projects);
	proj.insert(demandesDatasIndex, demandes);
	proj.insert(attributionDatasIndex, attributions);

	qDebug() << "encoding objects to json ended";

	QJsonDocument doc(proj);
	QByteArray datas = doc.toJson();

	if(!out.open(QIODevice::WriteOnly)){
		QMessageBox::critical(this,
							  "erreur lors de l'enregistrement du projet.",
							  "Impossible d'ouvrir le fichier en écriture.");
		return false;
	}

	qint64 w_stat = out.write(datas);
	out.close();

	qDebug() << "file wrinting ended";

	if(w_stat < 0){
		return false;
	}

	return true;


}

bool MainWindow::saveProjectAs(){

	QString saveFileName = getSaveFileName(".aspa", "projets aspa (*.aspa)", "", true);

	if(saveFileName == ""){
		return false;
	}

	QJsonObject projects = _projectsModel->representation();
	QJsonObject demandes = _demandModel->representation();
	QJsonObject attributions = _attributionModel->representation();

	QJsonObject proj;

	proj.insert(projectsDatasIndex, projects);
	proj.insert(demandesDatasIndex, demandes);
	proj.insert(attributionDatasIndex, attributions);

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

	_projectFile = saveFileName;

	if(w_stat < 0){
		return false;
	}

	return true;

}

void MainWindow::importProjects(){

	QString fileName = getImportFileName("fichiers csv (*.csv *.dat *.txt)",
									   QDir::homePath());

	if(fileName == ""){
		return;
	}

	QFile in(fileName);

	if(!in.open(QIODevice::ReadOnly)){
		QMessageBox::warning(this,
							 "Impossible d'importer le fichiers",
							 QString("Le fichier %1 n'a pas pût être importé.").arg(fileName));
		return;
	}

	QString datas(in.readAll());

	_projectsModel->parseCsvString(datas);

}

void MainWindow::importDemands(){

	QString fileName = getImportFileName("fichiers csv (*.csv *.dat *.txt)",
									   QDir::homePath());

	if(fileName == ""){
		return;
	}

	QFile in(fileName);

	if(!in.open(QIODevice::ReadOnly)){
		QMessageBox::warning(this,
							 "Impossible d'importer le fichiers",
							 QString("Le fichier %1 n'a pas pût être importé.").arg(fileName));
		return;
	}

	QString datas(in.readAll());

	_demandModel->parseCsvString(datas);
}

bool MainWindow::doAttribution(){

	QVector<Attribution> attr;


	//TODO: better algoirthms management

	if(ui->algosComboBox->currentText() == "hongroise"){
		attr = Attributor::hungarianAlgorithm(*_projectsModel,
											  *_demandModel);
	} else {
		attr = Attributor::directAttribution(*_projectsModel,
											 *_demandModel);
	}


	if(attr == QVector<Attribution>()){
		return false;
	}

	_attributionModel->setAttributionList(attr);

	ui->tabWidget->setCurrentIndex(3);

	return true;
}

void MainWindow::forwardInfoText(){
	emit(newInfosText(ui->infosTextEdit->toPlainText()));
}
void MainWindow::setTextToInfos(QString const& text){
	if(text != ui->infosTextEdit->toPlainText()){
		ui->infosTextEdit->setPlainText(text);
	}
}

bool MainWindow::exportAttributionAsHtml(){

	if(_attributionModel == nullptr){
		return false;
	}

	QString htmlText = _attributionModel->toHtml();
	QByteArray datas = htmlText.toUtf8();

	QString htmlFile = getSaveFileName(".html", "html files (*.html *.htm)", "");

	if(htmlFile == ""){
		return false;
	}

	QFile out(htmlFile);

	if(!out.open(QIODevice::WriteOnly)){
		QMessageBox::critical(this,
							  "erreur lors de l'export du fichier html",
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
bool MainWindow::exportAttributionAsPdf(){


	if(_attributionModel == nullptr){
		return false;
	}

	QString htmlText = _attributionModel->toHtml();
	QString fileName = getSaveFileName(".pdf", "pdf files (*.pdf)", "", true);

	QPrinter printer(QPrinter::PrinterResolution);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPaperSize(QPrinter::A4);
	printer.setOutputFileName(fileName);

	QTextDocument doc;
	doc.setHtml(htmlText);
	doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
	doc.print(&printer);

	return true;
}

QString MainWindow::getSaveFileName(QString defaultExt,
								 QString filter,
								 QString dir,
								 bool forceExtension){

	QDir ddir(dir);

	QFileDialog fd(this);
	fd.setWindowTitle(tr("exporter l'attribution en html"));
	fd.setDefaultSuffix(defaultExt);
	fd.setDirectory((ddir.exists() && dir != "") ? dir : QDir::homePath());
	fd.setNameFilter(filter);

	selection:

	int code = fd.exec();

	if(code == QDialog::Rejected){
		return "";
	}

	QString saveFileName = fd.selectedFiles().first();

	if(forceExtension){
		if(!saveFileName.endsWith(defaultExt)){
			saveFileName += defaultExt;
		}
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

	return saveFileName;

}


QString MainWindow::getImportFileName(QString filter,
									QString dir){

	QFileDialog fd(this);
	fd.setWindowTitle(tr("Choisir un fichier à importer"));
	fd.setFileMode(QFileDialog::ExistingFile);
	//fd.setDefaultSuffix(".aspa");
	fd.setDirectory(dir);
	fd.setNameFilter(filter);

	//selection:

	int code = fd.exec();

	if(code == QDialog::Rejected){
		return "";
	}

	QString openFileName = fd.selectedFiles().first();

	return openFileName;

}

void MainWindow::showAboutDialog(){

	AboutDialog dialog(this);

	dialog.exec();

}


void MainWindow::quit(){
	qApp->quit();
}
