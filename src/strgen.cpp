#include <iostream>
#include "randomgen.h"

using namespace std;

int main(int argc, char *argv[]) {
	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);

	int N = gsl_ran_binomial(r, );

















	//close rng
	gsl_rng_free(r);
	return 0;
}
