#ifndef ATTRIBUTIONMODEL_H
#define ATTRIBUTIONMODEL_H

#include <QObject>
#include <QAbstractTableModel>

#include <QVector>

#include "attributor.h"
#include "demandlistmodel.h"
#include "projectlistmodel.h"

class AttributionModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	static const char* titleIndex;
	static const char* infosIndex;
	static const char* attrIndex;

public:
	AttributionModel(DemandListModel* demands,
					 ProjectListModel* projects,
					 QVector<Attribution> attribution,
					 QObject* parent = nullptr);

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return _attributions.size(); }
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const{ (void) parent; return 2; }
	virtual QVariant data(const QModelIndex & index,
				  int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int column,
						Qt::Orientation orientation,
						int role = Qt::DisplayRole) const;

	QString toHtml() const;

	QString titre() const{ return _titre; }
	QString infos() const{ return _infos; }

	QJsonObject representation() const;
	void parseJsonObject(QJsonObject const& rep = QJsonObject());

signals:

	void titleChanged(QString title);
	void infosChanged(QString infos);

public slots:

	void setTitre(QString titre);
	void setInfos(QString infos);

	void setAttributionList(QVector<Attribution> const& attr);
	void clearAttributionList();
	void clearAll();

protected:

	DemandListModel* _demands;
	ProjectListModel* _projects;
	QVector<Attribution> _attributions;

	QString _titre;
	QString _infos;

};

#endif // ATTRIBUTIONMODEL_H
