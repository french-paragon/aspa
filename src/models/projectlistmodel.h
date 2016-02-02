#ifndef PROJECTLISTMODEL_H
#define PROJECTLISTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QVector>
#include <QSet>

#include <QJsonObject>

struct ProjectTuple{
	size_t index {0};
	QString name {QObject::tr("nouveau projet")};
	QVector<QString> additionalVars;
};

class ProjectListModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	static const char* indexColText;
	static const char* nameColText;

	static const char* idIndex;
	static const char* nameIndex;
	static const char* nextInsertIdIndex;
	static const char* additionalVarsIndex;
	static const char* tuplesIndex;

public:
	ProjectListModel(QObject * parent,
					 QJsonObject const& rep = QJsonObject());

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return _tuples.size(); }
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return 2 + _additionalVariablesName.size(); }
	virtual QVariant data(const QModelIndex & index,
				  int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex & index,
						 const QVariant & value,
						 int role = Qt::EditRole );
	virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

	QJsonObject representation() const;

	void emptyTuples();
	void parseJsonObject(QJsonObject const& rep = QJsonObject());
	void insertProjectTuple(ProjectTuple const& tuple);
	void removeSelectedTuples(QModelIndexList const& selecteds);

	QString findProjectNameById(int id);

signals:

	void changedDatas();

public slots:

	void createTuple();


protected:

	QJsonObject representTuple(ProjectTuple const& tuple) const;
	ProjectTuple parseTuple(QJsonObject const& tuple, bool & ok);

protected:

	mutable QVector<ProjectTuple> _tuples;
	QSet<int> _usedIndexes;
	QVector<QString> _additionalVariablesName;
	int _nextInsertId;
};

#endif // PROJECTLISTMODEL_H
