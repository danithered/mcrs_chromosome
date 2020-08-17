#include <iostream>
#include "randomgen.h"
#include "ca.h"

using namespace std;

gsl_rng * r;

int main(int argc, char *argv[]) {
    //initialise rng
    time_t timer;
    r = (gsl_rng *) gsl_rng_alloc (gsl_rng_mt19937);
    gsl_rng_set(r, time(&timer));

    cadv::CellAut automata(5,5);

    automata.neighInic(VON_NEUMANN_NEIGH, cadv::torus);
    //automata.rUpdate(1);

    //close rng
    gsl_rng_free(r);


    return 0;
} 
