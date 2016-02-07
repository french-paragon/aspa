#ifndef ATTRIBUTOR_H
#define ATTRIBUTOR_H

#include <QVector>

#include "demandlistmodel.h"
#include "projectlistmodel.h"

struct Attribution{
	int projetId;
	int groupId;
};

inline bool operator== (Attribution const& a1, Attribution const& a2){
	return a1.projetId == a2.projetId && a1.groupId == a2.groupId;
}

namespace Attributor
{

	/**
	 * @brief hungarianAlgorithm will try to match N groups with N projects, minimizing a given score function depending of the rank the group gave to a project.
	 * @param projects The list of the projects.
	 * @param demandes The list of the groups.
	 * @param rankToScore convert the rank of a project to a score.
	 * @return A list of attributions.
	 *
	 * L'implementation est la version matricielle avec un temps maximum d'execution de O(N^4), suffisant dans le cas d'une classe ou d'une petite PME.
	 */
	QVector<Attribution> hungarianAlgorithm(ProjectListModel const& projects,
												   DemandListModel const& demandes,
												   int (*rankToScore)(int) = nullptr);

	/**
	 * @brief directAttribution will give as much first choices as possible between the groups, then second, then third...
	 * @param projects The list of the projects.
	 * @param demandes The list of the groups.
	 * @return A list of attributions.
	 *
	 * L'algorithm a une complexité de O(N*M) (avec M la taille de la liste de préférence).
	 * Il donnera la solution optimal au problème d'attribution si les poids sont égaux
	 * au rang du projet obtenus et que l'on cherche à minimiser la sommes des poids.
	 */
	QVector<Attribution> directAttribution(ProjectListModel const& projects,
												  DemandListModel const& demandes);

}

#endif // ATTRIBUTOR_H
