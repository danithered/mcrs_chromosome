#ifndef _MYPARAMS_
#define _MYPARAMS_
 
#include <iostream>
#include <cstring>
#include <fstream>
 
#define MAXLEN 300
 
extern int par_noEA;

extern int par_maxtime;
extern int par_ncol;
extern int par_nrow;
extern int par_output_interval;

extern char par_ID[255];
extern char par_str_pool[255];

extern double par_init_grid;
extern double par_death;
extern double par_diffusion_rate;
extern double par_ll;
extern double par_sigma;
extern double par_claimEmpty;
extern double par_substitution;
extern double par_insertion;
extern double par_deletion;
extern double par_g;
extern double par_b1;
extern double par_b2;


//Functions 
int paramsToFile(char* filename);
int Args(int argc, char **argv);

#endif
