#include <iostream>
#include <fstream>
#include <string>
extern "C" {
#include <ViennaRNA/fold.h>
#include <ViennaRNA/utils/basic.h>
}
#include "randomgen.h"

char bases[] = "AGCU"; 

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

