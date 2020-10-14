#include <iostream>
#include "randomgen.h"
#include "ca.h"
#include "parameters.h"
#include "rnarep.h"

using namespace std;

gsl_rng * r;

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
	cadv::CellAut automata(par_nrow, par_ncol);

	rnarep::CellContent::patterns.readFile(par_str_pool); //read in pattern file

	automata.neighInic(MARGOLUS_NEIGH, cadv::torus, 0);
	automata.neighInic(VON_NEUMANN_NEIGH, cadv::torus, 1);
	automata.neighInic(VON_NEUMANN_NEIGH, cadv::torus, 2);
	automata.rUpdate(1);

	//close rng
	gsl_rng_free(r);


	return 0;
} 
