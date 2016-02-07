#include<munkres.h>

#include<eigen3/Eigen/Core>
#include<eigen3/Eigen/Dense>

#include<algorithm>

#include <assert.h>

using namespace Eigen;
using namespace std;

inline void step0(MatrixXi & weight,
		   int & step);

inline void step1(MatrixXi & weight,
		   vector<int> & stared0,
		   vector<bool> &primed0,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step);

inline void step2(MatrixXi & weight,
		   vector<int> & stared0,
		   vector<bool> & primed0,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step,
		   int & uCov0);

inline void step3(MatrixXi & weight,
		   vector<int> & stared0,
		   vector<bool> & primed0,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step,
		   int & uCov0);

inline void step4(MatrixXi & weight,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step);

#define COL_NOT_SET -1

/**
 * @brief munkres run the munkres alogrithm to solve the attribution for integer.
 * @param weight the weight matrix.
 * @return a vector containing the attributed tasks and worker encoded with the formula (row*n_cols + col).
 */
std::vector<int> munkres(MatrixXi pWeight){

	MatrixXi weight = pWeight;

	if(weight.rows() > weight.cols()){
		weight = weight.transpose();
	}

	vector<int> stared0(weight.rows(), COL_NOT_SET);
	vector<bool> primed0(weight.cols()*weight.rows(), false);

	vector<bool> coveredCol(weight.cols(), false);
	vector<bool> coveredRow(weight.rows(), false);

	bool end = false;
	int step = 0;
	int uCov0;

	while(!end){
		switch(step){
		case 0:
			step0(weight,
				  step);
			break;
		case 1:
			step1(weight,
				  stared0,
				  primed0,
				  coveredCol,
				  coveredRow,
				  step);
			break;
		case 2:
			step2(weight,
				  stared0,
				  primed0,
				  coveredCol,
				  coveredRow,
				  step,
				  uCov0);
			break;
		case 3:
			step3(weight,
				  stared0,
				  primed0,
				  coveredCol,
				  coveredRow,
				  step,
				  uCov0);
			break;
		case 4:
			step4(weight,
				  coveredCol,
				  coveredRow,
				  step);
			break;
		default:
			end = true;
			break;
		}
	}

	return stared0;

}

void step0(MatrixXi & weight,
		   int & step){
	//remove the min of each row from each row
	for(int i = 0; i < weight.rows(); i++){
		weight.row(i) -= MatrixXi::Constant(1, weight.cols(), weight.row(i).minCoeff());
	}

	//remove the min of each column from each column
	for(int i = 0; i < weight.cols(); i++){
		weight.col(i) -= MatrixXi::Constant(weight.rows(), 1, weight.col(i).minCoeff());
	}

	//next step is 1:
	step = 1;
}

void step1(MatrixXi & weight,
		   vector<int> & stared0,
		   vector<bool> & primed0,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step){

	bool hasFoundANew0 = false;
	bool hasNZerosSelected = true;

	for(int i = 0; i < weight.rows(); i++){
		for(int j = 0; j < weight.cols(); j++){
			if(weight(i,j) == 0){//case we found a 0 !
				if(stared0[i] == COL_NOT_SET){
					//if there's no 0 selected in the row.
					//TODO: check if such a search structure don't degrade the complexity.
					if(find(stared0.begin(), stared0.end(), j) == stared0.end()){
						//if there's no 0 selected in the column.
						stared0[i] = j;
						hasFoundANew0 = true;
					}
				}
			}
		}
		if(stared0[i] == COL_NOT_SET){ //if the row is still without selected 0
			hasNZerosSelected = false;
		}
	}

	if(hasNZerosSelected){
		//go out of steps
		step = -1;
		return;
	}

	if(hasFoundANew0){
		//if we have found a new 0 we remove primes and couvertures.
		primed0 = vector<bool>(weight.cols()*weight.rows(), false);
		coveredCol = vector<bool>(weight.cols(), false);
		coveredRow = vector<bool>(weight.rows(), false);
	}

	step = 2;

}

void step2(MatrixXi & weight,
		   vector<int> & stared0,
		   vector<bool> & primed0,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step,
		   int & uCov0){

	//couvrir toutes les colones avec un 0 sélectioné.
	for(int i = 0; i < (int) stared0.size(); i++){
		if(stared0[i] != COL_NOT_SET){
			coveredCol[stared0[i]] = true;
		}
	}

	//chercher un 0 non couvert
	int i, j;
	restart:
	for(i = 0; i < weight.rows(); i++){
		for(j = 0; j < weight.cols(); j++){
			if(weight(i,j) == 0){//case we found a 0 !
				if(!coveredRow[i] && !coveredCol[j]){
					//the 0 is uncovered.
					primed0[i*weight.cols()+j] = true;

					if(stared0[i] == COL_NOT_SET){
						//case there's no selected 0 on this row then
						//we don't have the maximum numbers of 0 selected
						//got to step 3.
						uCov0 = i*weight.cols() + j;
						step = 3;
						return;
					}

					//else we uncover the column of the selected 0 and cover the line
					coveredCol[stared0[i]] = false;
					coveredRow[i] = true;

					goto restart; //we may have uncovered another 0 !;

				}
			}
		}
	}

	step = 4;

}

void step3(MatrixXi & weight,
			vector<int> & stared0,
			vector<bool> & primed0,
			vector<bool> & coveredCol,
			vector<bool> & coveredRow,
			int & step,
			int & uCov0){

	int n = weight.rows();

	vector<int> pZi(1, uCov0); //the series of zi
	pZi.reserve(n); //we prepare the capacity to avoid reallocations on the run.

	int i = 1;

	while(true){

		//asert that the series don't exceed the number of rows.
		assert(i <= n); //program will exit otherwise.

		if(i%2){ //i is not prime
			//we are looking for a selected 0 on the column of z_i-1 (if it doesn't exist we can stop).
			int col = pZi[i-1]%weight.cols();
			for(int r = 0; r < weight.rows(); r++){
				if(stared0[r] == col){
					pZi.push_back(r*weight.cols() + col);
					break;
				}
			}
			if( (int) pZi.size() == i){
				// we did not find any selected 0
				break;
			}
		} else { //i is prime
			//we a looking for a primed 0 on the row of z_i-1 (that should exist).
			volatile int row_const = pZi[i-1]/weight.cols();
			row_const *= weight.cols();
			for(int c = 0; c < weight.cols(); c++){
				if(primed0[row_const + c] == true){
					pZi.push_back(row_const + c);
					break;
				}
			}
		}

		i++;

	}

	//select the primed 0s and unselect the others: this gives one more 0 selected.

	for(i = 0; i < (int) pZi.size(); i+=2){

		stared0[pZi[i]/weight.cols()] = pZi[i]%weight.cols();

	}

	//if we have found a new 0 we remove primes and couvertures.
	primed0 = vector<bool>(weight.cols()*weight.rows(), false);
	coveredCol = vector<bool>(weight.cols(), false);
	coveredRow = vector<bool>(weight.rows(), false);

	step = 1;

}

void step4(MatrixXi & weight,
		   vector<bool> & coveredCol,
		   vector<bool> & coveredRow,
		   int & step){

	int min = -1;

	for(int i = 0; i < weight.rows(); i++){
		for(int j = 0; j < weight.cols(); j++){
			if(weight(i,j) < min || min < 0){
				min = weight(i,j);
			}
		}
	}

	//add the min to all covered row and remove it from all non-covered column
	//equivalent remove it from all non-covered cell and add it to all double covered cells.
	for(int i = 0; i < weight.rows(); i++){
		for(int j = 0; j < weight.cols(); j++){
			if(coveredRow[i] && coveredCol[j]){
				weight(i,j) += min;
			} else if(!coveredRow[i] && !coveredCol[j]){
				weight(i,j) -= min;
			}
		}
	}

	step = 1;

}
