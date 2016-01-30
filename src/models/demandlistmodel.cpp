#include "demandlistmodel.h"

#include <QColor>
#include <QFont>
#include <QBrush>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <algorithm>


const char* DemandListModel::indexColText = "index";
const char* DemandListModel::nameColText = "Nom de groupe";

const char* DemandListModel::idIndex = "index";
const char* DemandListModel::namesIndex = "names";
const char* DemandListModel::choicesIndex = "choices";
const char* DemandListModel::nextInsertIdIndex = "nextId";
const char* DemandListModel::numberOfChoicesIndex = "n_choices";
const char* DemandListModel::additionalVarsIndex = "additionals";
const char* DemandListModel::tuplesIndex = "tuples";

const int DemandListModel::noChoice = -1;

DemandListModel::DemandListModel(QObject *parent, const QJsonObject &rep):
	QAbstractTableModel(parent)
{
	_numberOfChoices = 6; //6 is the new default.
	_nextInsertId = 1;
	_usedIndexes.insert(0);
	parseJsonObject(rep);
}

QVariant DemandListModel::data(const QModelIndex & index,
			  int role) const{

	int row = index.row();
	int col = index.column();

	switch(role){

	case Qt::DisplayRole:
	case Qt::EditRole:

		if(col == 0){

			return QVariant( (uint) _tuples[row].index);

		} else if (col == 1){

			return _tuples[row].names;

		} else if ((uint) col < 2 + _numberOfChoices){

			if(_tuples[row].preferences.size() <= col - 2){

				size_t old = _tuples[row].preferences.size();
				_tuples[row].preferences.resize(_numberOfChoices);

				for(int i = old; i < _tuples[row].preferences.size(); i++){
					_tuples[row].preferences[i] = noChoice;
				}

			}

			return (_tuples[row].preferences[col-2] == noChoice) ?
						( (role == Qt::DisplayRole) ? QVariant(QString("not set")) : QVariant(QString("")) ) :
						QVariant(_tuples[row].preferences[col-2]);

		} else {

			if(_tuples[row].additionalVars.size() < _additionalVariablesName.size()){
				_tuples[row].additionalVars.resize(_additionalVariablesName.size());
			}

			return _tuples[row].additionalVars[col-2-_numberOfChoices];

		}

	case Qt::BackgroundRole:
		if(col == 0){ // case we are in the first column
			if(row % 2){
				return QColor(244, 211, 121);
			} else {
				return QColor(231, 143, 30);
			}
		} else{
			if(row % 2){
				return QColor(255, 253, 199);
			} else {
				return QColor(241, 228, 166);
			}
		}

	case Qt::FontRole:

		if(col == 0){
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

QVariant DemandListModel::headerData(int column,
									 Qt::Orientation orientation,
									 int role) const{

	if(orientation == Qt::Horizontal) {

		switch(role){

		case Qt::DisplayRole:

			if(column == 0){
				return QString(tr(indexColText));
			} else if (column == 1){
				return QString(tr(nameColText));
			} else if ((uint) column < 2 + _numberOfChoices){
				return QString(tr("Choix %1")).arg(column-1);
			} else {
				return _additionalVariablesName[column-2-_numberOfChoices];
			}
			break;

		case Qt::BackgroundRole:

			return QColor(211, 128, 10);

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

bool DemandListModel::setData(const QModelIndex & index,
							  const QVariant & value,
							  int role ){

	int row = index.row();
	int col = index.column();

	if(role == Qt::EditRole){
		if(col > 0){
			if(value.type() == QVariant::String){
				if(col == 1){

					_tuples[row].names = value.toString();
					return true;

				} else if ((uint) col < 2 + _numberOfChoices){

					if(_tuples[row].preferences.size() <= col - 2){

						size_t old = _tuples[row].preferences.size();
						_tuples[row].preferences.resize(_numberOfChoices);

						for(int i = old; i < _tuples[row].preferences.size(); i++){
							_tuples[row].preferences[i] = noChoice;
						}
					}

					bool ok;
					int val = value.toInt(&ok);

					if(!ok){
						return false;
					}

					_tuples[row].preferences[col-2] = val;
					return true;

				} else {

					if(_tuples[row].additionalVars.size() < _additionalVariablesName.size()){
						_tuples[row].additionalVars.resize(_additionalVariablesName.size());
					}

					_tuples[row].additionalVars[col-2-_numberOfChoices] = value.toString();
					return true;

				}
			}
		}
	}

	return false;
}

Qt::ItemFlags DemandListModel::flags ( const QModelIndex & index ) const{

	int retour = (int) Qt::ItemIsEnabled;


	if(index.column() == 1) {
		retour |=  (int) Qt::ItemIsSelectable | (int) Qt::ItemIsEditable ;
	} else if (index.column() > 1) {
		retour |=  (int) Qt::ItemIsEditable ;
	}

	return (Qt::ItemFlags) retour;
}

QJsonObject DemandListModel::representTuple(DemandTuple const& tuple) const{

	QJsonObject rep;

	rep.insert(idIndex, QJsonValue( (int) tuple.index));
	rep.insert(namesIndex, tuple.names);

	QJsonArray additionals;

	for(QString var : tuple.additionalVars){
		additionals.push_back(QJsonValue(var));
	}

	rep.insert(additionalVarsIndex, additionals);

	QJsonArray choices;

	for (int choice : tuple.preferences){
		choices.push_back(QJsonValue(choice));
	}

	rep.insert(choicesIndex, choices);

	return rep;

}

QJsonObject DemandListModel::representation() const{

	QJsonObject rep;

	rep.insert(nextInsertIdIndex, QJsonValue(_nextInsertId));

	rep.insert(numberOfChoicesIndex, QJsonValue((int) _numberOfChoices));

	QJsonArray additionalVars;

	for(QString varName : _additionalVariablesName){
		additionalVars.push_back(QJsonValue(varName));
	}

	rep.insert(additionalVarsIndex, additionalVars);

	QJsonArray tuples;

	for(DemandTuple tuple : _tuples){
		tuples.push_back(representTuple(tuple));
	}

	rep.insert(tuplesIndex, tuples);

	return rep;

}

DemandTuple DemandListModel::parseTuple(QJsonObject const& tuple, bool & ok){

	DemandTuple rtuple;

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

	if(tuple.contains(namesIndex)){
		rtuple.names = tuple.value(namesIndex).toString();
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

	rtuple.preferences = QVector<int>(_numberOfChoices, noChoice);

	if(tuple.contains(choicesIndex)){
		if(tuple.value(choicesIndex).isArray()){

			QJsonArray array = tuple.value(choicesIndex).toArray();

			for(int i = 0; i < array.size() && i < rtuple.additionalVars.size(); i++){
				rtuple.additionalVars[i] = array.at(i).toInt(noChoice);
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

void DemandListModel::parseJsonObject(QJsonObject const& rep){

	if(rep.contains(nextInsertIdIndex)){
		_nextInsertId = rep.value(nextInsertIdIndex).toInt();
	}

	if(rep.contains(numberOfChoicesIndex)){
		_numberOfChoices = rep.value(numberOfChoicesIndex).toInt();
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

			emptyTuples();

			for(QJsonValue val: rep.value(tuplesIndex).toArray()){
				if(val.isObject()){
					bool ok;
					DemandTuple tuple = parseTuple(val.toObject(), ok);

					if(ok){
						insertDemandTuple(tuple);
					}
				}
			}
		}
	}
}


void DemandListModel::emptyTuples(){
	beginRemoveRows(QModelIndex(),0,rowCount()-1);
	_tuples.clear();
	_usedIndexes.clear();
	endRemoveRows();
}

void DemandListModel::insertDemandTuple(DemandTuple const& tuple){
	beginInsertRows(QModelIndex(), rowCount()-1, rowCount()-1);
	if(_usedIndexes.contains(tuple.index)){
		DemandTuple ntuple = tuple;
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
void DemandListModel::removeSelectedTuples(const QModelIndexList &selecteds){

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


void DemandListModel::createTuple(){
	DemandTuple tuple;
	tuple.additionalVars = QVector<QString>(_additionalVariablesName.size(), QString());
	tuple.preferences = QVector<int>(_numberOfChoices, noChoice);
	insertDemandTuple(tuple);
}
