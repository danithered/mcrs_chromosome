#include <iostream>
#include "randomgen.h"
#include "ca.h"
#include "parameters.h"
#include "rnarep.h"

using namespace std;


/* return values:
 0: Ok
 1: died
 -1: error in reading in argoments
 -2: couldnt open output 
*/
int main(int argc, char *argv[]) {
	//Argoments
	if ( Args(argc, argv) ) {
		return(-1);
	}

	//initialise rng
	time_t timer;
	if(std::strlen(par_seed_file)){
		std::cerr << "ERROR: this part of program is not functiioning right now, cant init rng" << std::endl;
	}
	else { // no seed file specified
		if(par_seed < 0){ // init with time
			randomszam_inic(time(&timer) + par_seed_plus, r);
		}
		else randomszam_inic(par_seed, r); // init with exact seed
	}

	//start to do stuff
	cadv::CellAut automata(par_nrow, par_ncol); //initialise automata

	rnarep::CellContent::patterns.readFile(par_str_pool); //read in pattern file

	automata.neighInic(MARGOLUS_NEIGH, cadv::torus, 0); //init diffusional neighbourhood for Toffoli-Margoulus algorithm
	automata.neighInic(par_Nmet, cadv::torus, 1); //init metabolic neighbourhood 
	automata.neighInic(par_Nrep, cadv::torus, 2); //init replication neighbourhood

	//load if needed
	if(std::strlen(par_load) > 0) automata.init_fromfile(par_load);

	//open output
	if(automata.openOutputs()) { //returns not 0 if fails
		gsl_rng_free(r);
		return -2;
	}

	//Running simulation
	if (automata.rUpdate(par_maxtime)){
		//close rng
		gsl_rng_free(r);

		return 0; // it has survived
	}
	else{	
		// died out
		//close rng
		gsl_rng_free(r);

		return 1;
	}

} 
