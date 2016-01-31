#include "attributionmodel.h"

#include <QColor>
#include <QFont>

AttributionModel::AttributionModel(DemandListModel* demands,
								   ProjectListModel* projects,
								   QVector<Attribution> attribution,
								   QObject* parent):
	QAbstractTableModel(parent)
{
	_demands = demands;
	_projects = projects;
	_attributions = attribution;
}

QVariant AttributionModel::data(const QModelIndex & index,
			  int role) const{

	int row = index.row();
	int col = index.column();

	switch(role){

	case Qt::DisplayRole:

		if(col == 0){
			int group = _attributions[row].groupId;
			return QString("%1. %2").arg(group)
					.arg(_demands->findGroupNameById(group));
		} else if(col == 1){
			int project = _attributions[row].projetId;
			return QString("%1. %2").arg(project)
					.arg(_projects->findProjectNameById(project));
		}

	case Qt::BackgroundRole:
		if(row % 2){
			return QColor(169, 91, 60);
		} else {
			return QColor(219, 167, 146);
		}

	default:
		return QVariant();

	}

	return QVariant();
}

QVariant AttributionModel::headerData(int column,
					Qt::Orientation orientation,
					int role) const{

	if(orientation == Qt::Horizontal){

		switch(role){

		case Qt::DisplayRole:
			if(column == 0){
				return tr("Nom du groupe");
			} else {
				return tr("Projet attribué");
			}

		case Qt::BackgroundRole:

			return QColor(144, 50, 13);

		case Qt::ForegroundRole:

			return QColor(255,255,255);

		case Qt::FontRole:
		{
			QFont boldFont;
			boldFont.setBold(true);
			return boldFont;
		}
		default:

			return QVariant();
		}

	}

	return QVariant();

}

QString AttributionModel::toHtml() const{

	QString out = "<html><head><meta charset=\"UTF-8\"></head><body><table style=\"border: 5px solid black;"
				  " border-collapse: collapse;\"><tr>";

	//table header
	for(int j = 0; j < columnCount(); j++){
		out += "<th style=\"padding: 15px; "
			   "color:" + headerData(j, Qt::Horizontal, Qt::ForegroundRole).value<QColor>().name()
				+ "; background:" + headerData(j, Qt::Horizontal, Qt::BackgroundRole).value<QColor>().name()
				+ "; \">" + headerData(j, Qt::Horizontal).toString() + "</th>";
	}

	out += "</tr>";

	//table body
	for(int i = 0; i < rowCount(); i++){
		out += "<tr>";
		for(int j = 0; j < columnCount(); j++){
			out += "<td style=\" padding: 15px; "
				   "color:" + data(index(i, j), Qt::ForegroundRole).value<QColor>().name()
					+ "; background:" + data(index(i, j), Qt::BackgroundRole).value<QColor>().name()
					+ "; \">" + data(index(i, j)).toString() + "</td>";
		}
		out += "</tr>";
	}

	out += "</table></body></html>";

	return out;
}
