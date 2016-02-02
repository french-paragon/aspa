#include "attributionmodel.h"

#include <QColor>
#include <QFont>

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>

const char* AttributionModel::titleIndex = "titre";
const char* AttributionModel::infosIndex = "infos";
const char* AttributionModel::attrIndex = "attributions";

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

	QString out = "<html><head><meta charset=\"UTF-8\"></head><body>";

	if(_titre != "") {
		out += QString("<h1>%1</h1><br>").arg(_titre);
	} else {
		out += "<h1>Attributions des projets</h1><br>";
	}

	if(_infos != ""){
		out += QString("<div>%1</div><br>").arg(_infos);
	}

	out += "<table style=\"border: 5px solid black;"
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

void AttributionModel::setTitre(QString titre){
	if(titre != _titre){
		_titre = titre;
		emit(titleChanged(_titre));
	}
}

void AttributionModel::setInfos(QString infos){
	if(infos != _infos){
		_infos = infos;
		emit(infosChanged(_infos));
	}
}

void AttributionModel::setAttributionList(QVector<Attribution> const& attr){
	qDebug() << "begin resetting attribution List. with old size = "
			 << _attributions.size() << "new size = " << attr.size();
	if(attr.size() > _attributions.size()){
		beginInsertRows(QModelIndex(), _attributions.size(), attr.size()-1);
		_attributions = attr;
		endInsertRows();
	} else if(attr.size() < _attributions.size()){
		beginRemoveRows(QModelIndex(), attr.size(), _attributions.size()-1);
		_attributions = attr;
		endRemoveRows();
	} else {
		_attributions = attr;
	}
	emit(dataChanged(index(0,0), index(_attributions.size()-1,1)));
	qDebug() << "end resetting attribution List.";
}

void AttributionModel::clearAttributionList(){
	if(_attributions.size() > 0){
		beginRemoveRows(QModelIndex(), 0, _attributions.size()-1);
		_attributions = QVector<Attribution>();
		endRemoveRows();
	}
}
void AttributionModel::clearAll(){
	setTitre("");
	setInfos("");
	clearAttributionList();
}

QJsonObject AttributionModel::representation() const{

	QJsonObject obj;

	obj.insert(titleIndex, _titre);
	obj.insert(infosIndex, _infos);

	QJsonArray attrs;

	for(int i = 0; i < _attributions.size(); i++){
		QJsonArray attr;
		attr.push_back(_attributions[i].groupId);
		attr.push_back(_attributions[i].projetId);

		attrs.push_back(attr);
	}

	obj.insert(attrIndex, attrs);

	return obj;

}
void AttributionModel::parseJsonObject(QJsonObject const& rep){

	if(rep.contains(titleIndex)){
		setTitre(rep.value(titleIndex).toString());
	} else {
		setTitre("");
	}

	if(rep.contains(infosIndex)){
		setInfos(rep.value(infosIndex).toString());
	} else {
		setTitre("");
	}

	QVector<Attribution> attr;

	if(rep.contains(attrIndex)){

		QJsonValue val = rep.value(attrIndex);

		if(val.isArray()){
			QJsonArray array = val.toArray();
			for(int i = 0; i < array.size(); i++){
				QJsonValue sub = array[i];
				if(sub.isArray()){
					QJsonArray subArray = sub.toArray();
					if(subArray.size() == 2){
						Attribution a;
						a.groupId = subArray[0].toInt();
						a.projetId = subArray[0].toInt();
						attr.push_back(a);
					}
				}
			}
		}
	}

	setAttributionList(attr);

}
