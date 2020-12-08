#include <iostream>
#include "randomgen.h"
#include "ca.h"
#include "parameters.h"
#include "rnarep.h"

using namespace std;

gsl_rng * r;

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
	r = (gsl_rng *) gsl_rng_alloc (gsl_rng_mt19937);
	gsl_rng_set(r, time(&timer));

	//start to do stuff
	cadv::CellAut automata(par_nrow, par_ncol); //initialise automata

	rnarep::CellContent::patterns.readFile(par_str_pool); //read in pattern file

	automata.neighInic(MARGOLUS_NEIGH, cadv::torus, 0); //init diffusional neighbourhood for Toffoli-Margoulus algorithm
	automata.neighInic(par_Nmet, cadv::torus, 1); //init metabolic neighbourhood 
	automata.neighInic(par_Nrep, cadv::torus, 2); //init replication neighbourhood

	if(automata.openOutputs()) { //open output
		gsl_rng_free(r);
		return -2;
	}

	//Running simulation
	automata.rUpdate(par_maxtime);

	//close rng
	gsl_rng_free(r);


	return 0;
} 
