#include <iostream>
#include "randomgen.h"

using namespace std;

gsl_rng * r;

int main(int argc, char *argv[]) {
    //initialise rng
    time_t timer;
    r = (gsl_rng *) gsl_rng_alloc (gsl_rng_mt19937);
    gsl_rng_set(r, time(&timer));




    cout << "Hello World";
  




    //close rng
    gsl_rng_free(r);


    return 0;
} 
