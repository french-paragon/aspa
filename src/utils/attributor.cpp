#include "attributor.h"

#include <algorithm>

#include "munkres.h"


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


QVector<Attribution> Attributor::hungarianAlgorithm(ProjectListModel const& projects,
											   DemandListModel const& demandes,
											   int (*rankToScore)(int)){

	QVector<int> demandesOrders(demandes.rowCount());
	QVector<Attribution> attributions;

	//build orders of demands, since munkres method don't break equalities with random method we first creat an order key.
	for(int i = 0; i < demandes.rowCount(); i++){
		demandesOrders[i] = i;
	}

	std::random_shuffle(demandesOrders.begin(), demandesOrders.end());

	//building matrix

	Eigen::MatrixXi weights(demandes.rowCount(), projects.rowCount());

	for(int i = 0; i < demandes.rowCount(); i++){
		for(int j = 0; j < projects.rowCount(); j++){

			int score = (int) demandes.numberOfChoices() + 1;

			for(int k = 1; k <= (int) demandes.numberOfChoices(); k++){
				if((int) projects.findProjectIdByRow(j) ==
						demandes.findPriorityIndexByRow(demandesOrders[i], k)){
					score = k;
					break;
				}
			}

			weights(i,j) = (rankToScore != nullptr) ? rankToScore(score) : score;
		}
	}

	std::vector<int> pre_attr = munkres(weights);

	//pre-allocate memory
	attributions.reserve(pre_attr.size());

	//case there were more projects than demands.
	if(projects.rowCount() >= demandes.rowCount()){
		//pre_attr represent the projects attributed to the i'th demand.
		for(int i = 0; i < (int) pre_attr.size(); i++){
			Attribution attr;
			attr.projetId = projects.findProjectIdByRow(pre_attr[i]);
			attr.groupId = demandes.findDemandIdByRow(demandesOrders[i]);
			attributions.push_back(attr);
		}
	} else {
		//pre_attr represent the demand attributed to the i'th project.
		for(int i = 0; i < (int) pre_attr.size(); i++){
			Attribution attr;
			attr.projetId = projects.findProjectIdByRow(i);
			attr.groupId = demandes.findDemandIdByRow(demandesOrders[pre_attr[i]]);
			attributions.push_back(attr);
		}
	}

	return attributions;

}
