#include "attributor.h"

#include <algorithm>



QVector<Attribution> Attributor::directAttribution(ProjectListModel const& projects,
											  DemandListModel const& demandes){

	QVector<int> demandesOrders(demandes.rowCount());
	QSet<int> attributedDemands;
	QSet<int> attributedProjects;
	QVector<Attribution> attributions;

	QVector<int> leftProjects;
	QVector<int> leftDemands;

	for(int i = 0; i < demandes.rowCount(); i++){
		bool ok;
		demandesOrders[i] = demandes.data(demandes.index(i, 0)).toInt(&ok);
		if(!ok){
			//TODO: Maybe a better error management.
			return QVector<Attribution>();
		}
	}

	std::random_shuffle(demandesOrders.begin(), demandesOrders.end()); //random shuffle group for fairness.

	for(unsigned int level = 1; level <= demandes.numberOfChoices(); level++){
		//attribute first choices fisrt, then second... up to the max choice level
		//above which we consider all choices are equals.
		for(int i = 0; i < demandesOrders.size(); i++){
			//iterate over all demands.
			if(attributedDemands.size() == demandesOrders.size() ||
					attributedProjects.size() == projects.rowCount()){
				//all demand are attributed.
				goto all_attributed;
			}
			if(attributedDemands.contains(demandesOrders[i])){
				continue;
			}

			int choiceOfLevel = demandes.findPriorityIndexById(demandesOrders[i],level);

			if( choiceOfLevel == DemandListModel::noChoice ){
				continue; //chance missed !
			}

			if(!attributedProjects.contains(choiceOfLevel)){
				//choice has not been attributed.
				//attribute it:
				Attribution attr;
				attr.projetId = choiceOfLevel;
				attr.groupId = demandesOrders[i];
				attributions.push_back(attr);
				attributedDemands.insert(demandesOrders[i]);
				attributedProjects.insert(choiceOfLevel);
			}
		}
	}

	//attribute the rest of the projects to the rest of the groups


	for(int i = 0; i < projects.rowCount(); i++){
		bool ok;
		int project = projects.data(demandes.index(i, 0)).toInt(&ok);
		if(!ok){
			//TODO: Maybe a better error management.
			return QVector<Attribution>();
		}
		if(!attributedProjects.contains(project)){
			leftProjects.push_back(project);
		}
	}

	for(int i = 0; i < demandes.rowCount(); i++){
		bool ok;
		int demand = demandes.data(demandes.index(i, 0)).toInt(&ok);
		if(!ok){
			//TODO: Maybe a better error management.
			return QVector<Attribution>();
		}
		if(!attributedDemands.contains(demand)){
			leftDemands.push_back(demand);
		}
	}

	//shuffle again.
	std::random_shuffle(leftProjects.begin(), leftProjects.end());
	std::random_shuffle(leftDemands.begin(), leftDemands.end());

	for(int i = 0; i < leftDemands.size() && i < leftProjects.size(); i++){
		Attribution attr;

		attr.projetId = leftProjects[i];
		attr.groupId = leftDemands[i];

		attributions.push_back(attr);
	}

	all_attributed:

	return attributions;

}
