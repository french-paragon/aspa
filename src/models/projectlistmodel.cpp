#include "projectlistmodel.h"

#include <QColor>
#include <QFont>
#include <QBrush>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <algorithm>

const char* ProjectListModel::indexColText = "index";
const char* ProjectListModel::nameColText = "nom";

const char* ProjectListModel::idIndex = "index";
const char* ProjectListModel::nameIndex = "name";
const char* ProjectListModel::nextInsertIdIndex = "next_index";
const char* ProjectListModel::additionalVarsIndex = "additionals";
const char* ProjectListModel::tuplesIndex = "tuples";

ProjectListModel::ProjectListModel(QObject *parent, const QJsonObject &rep):
	QAbstractTableModel(parent)
{
	_nextInsertId = 1;
	_usedIndexes.insert(0);
	parseJsonObject(rep);
}

QVariant ProjectListModel::data(const QModelIndex & index,
								int role) const{

	int row = index.row();
	int column = index.column();

	switch(role){

	case Qt::DisplayRole:
	case Qt::EditRole:

		if(column == 0){
			return QVariant( (uint) _tuples[row].index);
		} else if (column == 1){
			return _tuples[row].name;
		} else {
			return _tuples[row].additionalVars[column-2];
		}
	case Qt::BackgroundRole:
		if(column == 0){ // case we are in the first column
			if(row % 2){
				return QColor(135, 156, 71);
			} else {
				return QColor(183, 199, 133);
			}
		} else{
			if(row % 2){
				return QColor(199, 199, 133);
			} else {
				return QColor(230, 230, 180);
			}
		}
	case Qt::FontRole:

		if(column == 0){
			QFont boldFont;
			boldFont.setBold(true);
			return boldFont;
		} else {
			return QFont();
		}

	default:
		return QVariant();

	}

	return QVariant();
}

QVariant ProjectListModel::headerData(int column,
									  Qt::Orientation orientation,
									  int role) const{

	if(orientation == Qt::Horizontal) {

		switch(role){

		case Qt::DisplayRole:

			if(column == 0){
				return QString(tr(indexColText));
			} else if (column == 1){
				return QString(tr(nameColText));
			} else {
				return _additionalVariablesName[column-2];
			}
			break;

		case Qt::BackgroundRole:

			return QColor(80, 100, 25);

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

bool ProjectListModel::setData ( const QModelIndex & index,
			   const QVariant & value,
			   int role) {


	int row = index.row();
	int column = index.column();

	if(role == Qt::EditRole){
		if(column > 0){// indices ar not editables.
			if(value.type() == QVariant::String){
				if(column == 1){

					_tuples[row].name = value.toString();
					return true;

				} else if (column - 2 < _additionalVariablesName.size()){

					if(_tuples[row].additionalVars.size() < _additionalVariablesName.size()){
						_tuples[row].additionalVars.resize(_additionalVariablesName.size());
					}
					_tuples[row].additionalVars[column-2] = value.toString();
					return true;

				}
			}
		}
	}

	return false;
}
Qt::ItemFlags ProjectListModel::flags ( const QModelIndex & index ) const{

	int retour = (int) Qt::ItemIsEnabled;

	if(index.column() > 0) {
		retour |=  (int) Qt::ItemIsSelectable | (int) Qt::ItemIsEditable ;
	}
	return (Qt::ItemFlags) retour;

}

QJsonObject ProjectListModel::representTuple(ProjectTuple const& tuple) const{

	QJsonObject rep;

	rep.insert(idIndex, QJsonValue( (int) tuple.index));
	rep.insert(nameIndex, tuple.name);

	QJsonArray additionals;

	for(QString var : tuple.additionalVars){
		additionals.push_back(QJsonValue(var));
	}

	rep.insert(additionalVarsIndex, additionals);

	return rep;
}

QJsonObject ProjectListModel::representation() const{

	QJsonObject rep;

	rep.insert(nextInsertIdIndex, QJsonValue(_nextInsertId));

	QJsonArray additionalVars;

	for(QString varName : _additionalVariablesName){
		additionalVars.push_back(QJsonValue(varName));
	}

	rep.insert(additionalVarsIndex, additionalVars);

	QJsonArray tuples;

	for(ProjectTuple tuple : _tuples){
		tuples.push_back(representTuple(tuple));
	}

	rep.insert(tuplesIndex, tuples);

	return rep;

}


ProjectTuple ProjectListModel::parseTuple(QJsonObject const& tuple, bool & ok){

	ProjectTuple rtuple;

	ok = true;

	if(tuple.contains(idIndex)){
		int id = tuple.value(idIndex).toInt();

		if(_usedIndexes.contains(id)){
			rtuple.index = _nextInsertId;
		} else {
			rtuple.index = id;
		}

	} else {
		goto end_with_error;
	}
	if(tuple.contains(nameIndex)){
		rtuple.name = tuple.value(nameIndex).toString();
	}
	rtuple.additionalVars = QVector<QString>(_additionalVariablesName.size(), QString());

	if(tuple.contains(additionalVarsIndex)){
		if(tuple.value(additionalVarsIndex).isArray()){
			QJsonArray array = tuple.value(additionalVarsIndex).toArray();

			for(int i = 0; i < array.size() && i < rtuple.additionalVars.size(); i++){
				rtuple.additionalVars[i] = array.at(i).toString();
			}
		}
	}

	end:

	if(rtuple.index >= (uint) _nextInsertId){
		_nextInsertId = rtuple.index + 1;
	}

	return rtuple;

	end_with_error:

	ok = false;
	goto end;
	return rtuple;
}

void ProjectListModel::parseJsonObject(QJsonObject const& rep){

	if(rep.contains(nextInsertIdIndex)){
		_nextInsertId = rep.value(nextInsertIdIndex).toInt();
	}

	if(rep.contains(additionalVarsIndex)){
		if(rep.value(additionalVarsIndex).isArray()){
			QVector<QString> additionals;
			for(QJsonValue val: rep.value(additionalVarsIndex).toArray()){
				if(val.isString()) {
					additionals.push_back(val.toString());
				}
			}
			_additionalVariablesName = additionals;
		}
	}

	if(rep.contains(tuplesIndex)){
		if(rep.value(tuplesIndex).isArray()){

			QVector<ProjectTuple> tuples;
			emptyTuples();

			for(QJsonValue val: rep.value(tuplesIndex).toArray()){
				if(val.isObject()){
					bool ok;
					ProjectTuple tuple = parseTuple(val.toObject(), ok);

					if(ok){
						insertProjectTuple(tuple);
					}
				}
			}
		}
	}

}


void ProjectListModel::emptyTuples(){
	beginRemoveRows(QModelIndex(),0,rowCount()-1);
	_tuples.clear();
	_usedIndexes.clear();
	endRemoveRows();
}

void ProjectListModel::insertProjectTuple(ProjectTuple const& tuple){
	beginInsertRows(QModelIndex(), rowCount()-1, rowCount()-1);
	if(_usedIndexes.contains(tuple.index)){
		ProjectTuple ntuple = tuple;
		ntuple.index = _nextInsertId;
		_usedIndexes.insert(ntuple.index);
		_nextInsertId++;
		_tuples.push_back(ntuple);
	} else {
		_usedIndexes.insert(tuple.index);
		_tuples.push_back(tuple);
		if(tuple.index > (uint) _nextInsertId){
			_nextInsertId = tuple.index+1;
		}
		_tuples.push_back(tuple);
	}
	endInsertRows();
}
void ProjectListModel::removeSelectedTuples(const QModelIndexList &selecteds){

	QVector<int> rows;

	for(QModelIndex selected: selecteds) {
		if(!rows.contains(selected.row())){
			rows.push_back(selected.row());
		}
	}

	std::sort(rows.begin(), rows.end());

	for(int i = 0; i < rows.size(); i++){

		beginRemoveRows(QModelIndex(), rows[i]-i, rows[i]-i);
		_usedIndexes.remove(_tuples[rows[i]-i].index);
		_tuples.removeAt(rows[i]-i);
		endRemoveRows();
	}

}


void ProjectListModel::createTuple(){
	ProjectTuple tuple;
	tuple.additionalVars = QVector<QString>(_additionalVariablesName.size(), QString());
	insertProjectTuple(tuple);
}
