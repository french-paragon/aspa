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

signals:

public slots:

protected:

	DemandListModel* _demands;
	ProjectListModel* _projects;
	QVector<Attribution> _attributions;

};

#endif // ATTRIBUTIONMODEL_H
