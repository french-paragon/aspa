#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "mainwindow.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	setWindowTitle(tr("A propos"));
	ui->aboutLabel->setText(tr("%1 est un programe crée par paragon.")
							.arg(MainWindow::progName));
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
