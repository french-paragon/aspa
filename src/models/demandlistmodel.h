#ifndef DEMANDLISTMODEL_H
#define DEMANDLISTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QSet>

#include <QJsonObject>

struct DemandTuple{
	size_t index {0};
	QString names {QObject::tr("nouvelle équipe")};
	QVector<int> preferences;
	QVector<QString> additionalVars;
};

class DemandListModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	static const char* indexColText;
	static const char* nameColText;

	static const char* idIndex;
	static const char* namesIndex;
	static const char* choicesIndex;
	static const char* nextInsertIdIndex;
	static const char* numberOfChoicesIndex;
	static const char* additionalVarsIndex;
	static const char* tuplesIndex;

	static const int noChoice;

public:
	DemandListModel(QObject * parent,
					QJsonObject const& rep = QJsonObject());

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return _tuples.size(); }
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return 2 + _numberOfChoices + _additionalVariablesName.size(); }
	virtual QVariant data(const QModelIndex & index,
				  int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int column,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex & index,
						 const QVariant & value,
						 int role = Qt::EditRole );
	virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

	QJsonObject representation() const;

	void emptyTuples();
	void parseJsonObject(QJsonObject const& rep = QJsonObject());
	void insertDemandTuple(DemandTuple const& tuple);
	void removeSelectedTuples(QModelIndexList const& selecteds);

	unsigned int numberOfChoices() const { return _numberOfChoices; }
	//TODO: mapping avec les indexes (évent)
	QString findGroupNameById(int id) const;
	int findPriorityIndexById(int id, unsigned int level) const;

signals:

public slots:

	void createTuple();


protected:

	QJsonObject representTuple(DemandTuple const& tuple) const;
	DemandTuple parseTuple(QJsonObject const& tuple, bool & ok);

protected:

	mutable QVector<DemandTuple> _tuples;
	QSet<int> _usedIndexes;
	QVector<QString> _additionalVariablesName;
	int _nextInsertId;

	unsigned int _numberOfChoices;
};

#endif // DEMANDLISTMODEL_H
