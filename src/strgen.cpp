#include <iostream>
#include <vector>
#include <string.h>
#include "randomgen.h"

using namespace std;

int find_new_pos(std::vector<int> &vec, int max){
	int hanyadik = gsl_ran_binomial(r, 0.5, max) , p;

	if( vec.empty() ) return(hanyadik);
	
	for (std::vector<int>::iterator  d = vec.begin(); (!vec.empty())  && d != vec.end(); ++d) {
		if(hanyadik == *d) {
			hanyadik = find_new_pos(vec, max);
			break;
		}
	}
	return(hanyadik);
}

char bases[] = "AGCU"; 

int give_random_str(int str_length = 20, double prop_paired = 0.4, double prop_seq = 0.2){

	int N = gsl_ran_binomial(r, 0.5, str_length);

	int n_p = gsl_ran_binomial(r, prop_paired, N/2);
	int no_paired = n_p*2;
	std::string str;
	int n_points = N - no_paired;

	std::vector<int> pos;
	std::vector<char> base;

	if(no_paired){
		str.assign(no_paired, '0');
//		std::cout << "N=" << N << ", no_paired=" << no_paired << std::endl;

		str[0] = '(';

		if(0.0 < gsl_rng_uniform(r)){ //not interloop
			//arranging (s and )s
			int n_c=1, n_rc=0;
			std::vector<std::string::iterator> where;

			for(std::string::iterator ch=str.begin() + 1; ch!=str.end(); ++ch){
				//std::cout << "n_p: " << n_p << ", n_c: " << n_c << ", n_rc: " << n_rc << std::endl;
				//std::cout << ( (double) n_p-n_c)/( (double) n_p-n_rc) << std::endl;
				if(gsl_rng_uniform(r) < ( (double) n_p-n_c)/( (double) n_p-n_rc) ){
					*ch = '(';
					n_c++;
				}
				else{
					*ch = ')';
					n_rc++;
				}
			}
			//adding points to it
			//first adding loop bases
			for(std::string::iterator ch=str.begin(); (ch+1) != str.end(); ++ch){
				if(*ch == '(' && *(ch+1) == ')' ) {
					where.push_back(ch+1);
//					std::cout << "where: " << (int) ch - str.begin() << std::endl;
				}
			}
			for(; n_points > 0 && where.size() > 0 ; n_points-- ) {
				int which = gsl_rng_uniform_int(r, where.size());
				str.insert(where[which ], '.');
				for(auto w = where.begin(); w != where.end(); ++w) if( *w > where[which] ) (*w)++;
				where.erase(where.begin() + which);
			}
			//adding extra bases
			for(; n_points > 0; n_points--){
				str.insert(str.begin() + gsl_rng_uniform_int(r, str.length()-1) + 1, '.') ;
			}


		} 
		else { // interloop
		}

	}
	else {
		str.assign(N, '.');
//		std::cout << "N=" << N << ", no_paired=" << no_paired << std::endl;
	}

	std::cout << str << std::endl;

	if(n_points = N - no_paired) {

		for(int n_clues = gsl_ran_binomial(r, prop_seq, n_points); n_clues--;){
			//int hanyadik = gsl_ran_binomial(r, 0.5, n_points) , p;
			//for (auto d = pos.begin(); d != NULL && d != pos.end(); ++d) 
			//for( p=0; hanyadik = (str[p] == '.'):(--hanyadik)?hanyadik ; p++) {};
			//pos = ;
			pos.push_back( find_new_pos(pos, n_points-1) );
			base.push_back( bases[gsl_rng_uniform_int(r, 4)] );
//			std::cout << n_clues << ": " << pos.back() << base.back() << std::endl;
		}
		for(auto po = pos.begin(); po != pos.end(); ++po){
			int it, point_found;
			for(it = 0, point_found = -1 ; point_found < *po && it != str.length(); it++) if(str[it] == '.') point_found++; 
			*po = it;
//			std::cout << *po << std::endl;
		}
	}

	for(int i = 0; i < pos.size(); i++) std::cout << pos[i] << base[i] << std::endl;
	//its output starts from 1! Not 0!

	return 0;
}

#include "rnarep.h"
#include "annot.h"
#include "parameters.h"
#include "dv_tools.h"

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
	int max_mut = 50;
	int *results;
	char seq_file[] = "IN/sample500.txt";

	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);

	//give_random_str();

	//open seq file
	std::ifstream file(seq_file);
	if(!file.is_open()) std::cerr << "ERROR: file (" << seq_file << ") not found!" << std::endl;
	
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


