#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
extern "C" {
#include <ViennaRNA/fold.h>
#include <ViennaRNA/utils/basic.h>
}
#include "randomgen.h"

#include <vector>
#include "rnarep.h"
#include "annot.h"
#include "parameters.h"
#include "dv_tools.h"

using namespace std;

char bases[] = "AGCU"; 
/*
int main(int argc, char *argv[]){
	int lambda = 45, no_seqs=5000000;
	char seq_file[] = "OUT/randseqs.txt";


	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);

	//open seq file
	std::ofstream file(seq_file, std::ios_base::app);
	if(!file.is_open()) std::cerr << "ERROR: file (" << seq_file << ") not found!" << std::endl;
	
	//intit vars
	std::string seq;
	double mfe;
	char *str;
	str = (char *) vrna_alloc(sizeof(char) *  200 );

	//start computing
	for(; no_seqs--; ){
		//get a sequence
		seq.clear();
		for(int rb=gsl_ran_poisson(r, lambda); rb--;) seq.push_back(bases[gsl_rng_uniform_int(r, 4)]);
		
		//compute str and mfe
		mfe = vrna_fold(seq.c_str(), str);
		file  << seq << '\t' << str << '\t' << seq.length() << '\t' << mfe << std::endl; 
	}






	file.close();
	delete [] (str);

	//close rng
	gsl_rng_free(r);
	return 0;
}
*/


int main(int argc, char *argv[]){
	//Argoments
	if ( Args(argc, argv) ) {
		return(-1);
	}

	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);






	//stuff
	std::string seq_file_start("OUT/randseqs_ea"), seq_file, seq;
	rnarep::CellContent::patterns.readFile(par_str_pool); //read in pattern file
	rnarep::CellContent replicator;
	int no_seqs = 500, lambda = 45;
	long long int type;

//	rnarep::CellContent::patterns.printRules();

	//create output file streams
	std::vector<std::ofstream> files;
	std::ofstream tempof;
	double no_types = std::pow(2.0, (double) par_noEA);
	for(int of = 0; of < par_noEA; of++){ 
		seq_file.erase();
		seq_file = seq_file_start;
		seq_file += std::to_string(of);
		seq_file += ".txt";

		tempof.open(seq_file, std::ios_base::app);
		files.push_back(std::move(tempof));
//		files[of] << "vmi";
		//if(!files[of].is_open()) std::cerr << "ERROR: file (" << seq_file << ") not found!" << std::endl;
	}

	for(; no_seqs--; ){
		//get a sequence
		seq.clear();
//		std::cout << "seq cleared" << seq << std::endl;
		for(int rb=gsl_ran_poisson(r, lambda); rb--;) {
//			std::cout << "add base" << std::endl;
			seq.push_back(bases[gsl_rng_uniform_int(r, 4)]);
		}
//		std::cout << "seq is " << seq << std::endl;
		replicator = seq;

//		std::cout << replicator.getR() <<  std::endl;

		//put it in file
		type = replicator.get_type();
		if(type < no_types) { //dont forget to delete this condition! it just slows it down...
			if(type && (type % 2 == 0) ) files[(int) std::pow(type, 0.5) ] << seq << '\t' << replicator.get_str() << '\t' << seq.length() << '\t' << replicator.get_mfe() << std::endl;
		}
		else {
			std::cerr << "ERROR: calculation of number of types (" << type << " - max: " << no_types << ") were wrong!" << std::endl;
		}
		
		//die
		replicator.die();
	}








	for(auto f = files.begin(); f != files.end(); f++) (*f).close();
	//close rng
	gsl_rng_free(r);
	return 0;
}

