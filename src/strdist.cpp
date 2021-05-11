#include <iostream>
#include <vector>
#include <string.h>
#include "randomgen.h"
#include "rnarep.h"
#include "annot.h"
#include "parameters.h"
#include "dv_tools.h"

using namespace std;

char bases[] = "AGCU"; 

int* map_mutation_neighbourhood(std::string &startseq, char* input_file, int mut_dist = 1, int no_repeats = 100, double prop_of_sub = 0.8){
	std::string mutseq;
	int *transitions;

	rnarep::CellContent::patterns.readFile(input_file); //read in pattern file

	rnarep::CellContent replicator, mutant;

	//allocate memory for storing results
	int no_possible_outcomes = std::pow(2, par_noEA)-1;
//	std::cout << "no_possible_outcomes: " << no_possible_outcomes << ", par_noEA: " << par_noEA << std::endl;

	//first is type of original
	transitions = new int [no_possible_outcomes+1];

	for(int ea = 0; ea < (no_possible_outcomes+1); ea++) {
		transitions[ea] = 0;
	}

	//assign the seq to replicator
	replicator = startseq;

	//get enzim information from replicator
	transitions[0] = replicator.get_type();

//	std::cout << "alap seq: " << startseq << ", str: " << replicator.get_str() << ", type: " << transitions[0] << std::endl;
	transitions++; //step forward pointer for conveniance - dont forget to step it back!

	for(int no_mut_rep = no_repeats; no_mut_rep--;){
		//mutate it
		mutseq = startseq;
		
		for(int no_mut_steps = 1; no_mut_steps <= mut_dist; no_mut_steps++){
			double rand = gsl_rng_uniform(r);
			if(rand < prop_of_sub){ //substitution
			int target = gsl_rng_uniform_int(r, mutseq.length() );
			mutseq[ target  ] = bases[( rnarep::RNAc2i( rnarep::RNAc2cc( (char) mutseq[target] ) ) + gsl_rng_uniform_int(r, 3) + 1) % 4];
			} 
			else if(rand < ((1+prop_of_sub)/2) ){ //addition
				int target = gsl_rng_uniform_int(r, mutseq.length()+1);
				mutseq.insert(mutseq.begin() + target, bases[gsl_rng_uniform_int(r,4)]);
			}
			else{ //deletion
				mutseq.erase(mutseq.begin() + gsl_rng_uniform_int(r, mutseq.length()));
			}
//		
//			std::cout << "mutated" << std::endl;

			//mutated
			mutant = mutseq;
			int mtype = mutant.get_type();
			if( transitions[ mtype ] > no_mut_steps || !transitions[ mtype ] ) {
				transitions[ mtype ] = no_mut_steps; 
//				std::cout << "in repeat " << no_mut_rep << " at mutational distance " << no_mut_steps << " found a mutant of type " << mtype << std::endl;
			}
		} //mutations

		//calculate mutation
		//mutant = mutseq;
		//transitions[ mutant.get_type() ]++; //old version to register number of transition, not closest distance
	} //mutants repetiton

	//outputting
//	for(int ea = 0; ea < no_possible_outcomes; ea++) std::cout << "transition to " << ea << ": " << transitions[ea] << std::endl;

	//delete
	//delete [] (transitions);

	transitions--; //step back for it to start with type of original
	return transitions;
}

int main(int argc, char *argv[]){
	//Argoments
	if ( Args(argc, argv) ) {
		return(-1);
	}

	int max_mut = 50;
	int *results;
	//char seq_file[] = "IN/sample500.txt";

	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);

	//open seq file
	std::ifstream file(par_load);
	if(!file.is_open()) std::cerr << "ERROR: file (" << par_load<< ") not found!" << std::endl;
	
	//get a sequence
	//std::string seq("GUAAUUGCCGACUAUUACGCUGAUGUGACGACCAUCCUUCCGUCCCAAG");
	std::string seq;
	while( std::getline(file, seq) ){

		//for(int no_muts = max_mut; --no_muts;) 
		results = map_mutation_neighbourhood(seq, par_str_pool, max_mut);

		int rmax = std::pow(2, par_noEA);
		for(int r = 0; r < rmax; r++) std::cout << results[r] << "\t";
		std::cout << std::endl;
	}

	delete[] results;






	file.close();

	//close rng
	gsl_rng_free(r);
	return 0;
}

