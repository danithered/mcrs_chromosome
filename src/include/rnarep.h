#ifndef _RNAREP_
#define _RNAREP_

#include <string.h>
#include <vector>
#include <cmath>

extern "C" {
#include <ViennaRNA/fold.h>
#include <ViennaRNA/utils/basic.h>
}

#include "randomgen.h"
#include "parameters.h"
#include "dv_tools.h"
#include "annot.h"

namespace rnarep {
	extern char bases[5];

	double EoptCalc(int n);
	double cvalCalc(int n);
	double length_depCalc(int n);

	char RNAc2cc(char rna){
		/* Function by: Enrico Sandro Colizzi */
		switch (rna) {
			case 'A':
				return 'U';
			case 'C':
			  	return 'G';
			case 'G':
				return 'C';
			case 'U':
				return 'A';
		      	default:
			        //fprintf(stderr,"RNAc2cc: I got a non RNA character (%c)\n", rna);
				std::cerr << "RNAc2cc: I got a non RNA character (" << rna <<")" << std::endl;
				return '\0';
		}
			
	}

	int RNAc2i(char rnachar){
		switch (rnachar) {
			case 'G':
			        return 0;
			case 'C':
				return 1;
			case 'U':
				return 2;
			case 'A':
				return 3;
			default:
				std::cerr << "RNAc2i: I got a non RNA character (" << rnachar << ")" << std::endl;
				return -1;
		}
	}

	class CellContent{
		public:
			double value1; //temporary, just for testing

			double Pdeg; //degradation rate
			bool empty; // is this cell empty or not

			static dvtools::quickPosVals Eopt;
			static dvtools::quickPosVals cval;
			static dvtools::quickPosVals length_dep;
			static dvtools::quickPosVals m_sigma;
			static dv_annot::PatternPool patterns;

			//initialiser
			CellContent(){	
				int len=0;
				
				//initialise array for seq
				//if ( !(seq = new char [MAXLEN]) ) std::cerr << "ERROR: could not allocate space for seq" << std::endl;
				//seq = new char [MAXLEN]; 
				//std::memset(seq, '\0', MAXLEN);

				//add random values for seq
				if(gsl_rng_uniform(r) < par_init_grid) {
					len = gsl_rng_uniform_int(r, 10) + 1; //initiate with n bases, n is [1,11] equal distribution
					for(int b=0; b < len; b++){
						seq.push_back( rnarep::bases[gsl_rng_uniform_int(r,4)] );
					}
					annotate();
				}
				else { //empty
					die();
				}

				seq.reserve(MAXLEN);
				//ins.reserve(MAXLEN+1);
				//subs.reserve(MAXLEN+1);
				//dels.reserve(MAXLEN);

				
				// allocate memory for MFE structure (length + 1)
				str = (char *) vrna_alloc(sizeof(char) * ( MAXLEN  + 1));

				// allocate memory for enzymaitc activities
				a = new double [par_noEA];
			 }

			CellContent(std::string input_str){
				seq.reserve(MAXLEN);
				//ins.reserve(MAXLEN+1);
				//subs.reserve(MAXLEN+1);
				//dels.reserve(MAXLEN);

				seq = input_str;
				// allocate memory for MFE structure (length + 1)

				str = (char *) vrna_alloc(sizeof(char) * ( MAXLEN  + 1));

				// allocate memory for enzymaitc activities
				a = new double [par_noEA];

				if(seq.length()){
					annotate();
				}
				else {
					die();
				}
			}

			~CellContent(){
				//delete [] (seq);
				//cleanup memory
				free(str);
				delete [] (a);
			}

			void die(){
				if (!empty) {
					empty = true;
					seq.clear();
					Pdeg = 0;
					//R = 0;
					annot_level = 0;
					//for (int act = 0; act < par_noEA; act++) a[act] = 0;
				}
			}

			void replicate(const CellContent &templ){
				int tsize = templ.seq.size();
				int tsizeminus = tsize - 1;
				
				seq.resize(tsize);
/*				
				//ins, dels
				dels.clear();
				ins.clear();
				subs.clear();

				// Calculate positions of insertions
				// insertions can appear at length + 1 positions. For sequence GCA (length = 3) 4 insertions available at positions x: xGxCxAx
				
				for(int num = 0, number_of_muts = gsl_ran_binomial(r, par_insertion, tsize+1); num < number_of_muts; num++){
					if(number_of_muts == tsize + 1) { // if mutations will be at every possible position
						for(int until = 0; until < number_of_muts; until++){
							ins.push_back(until);
						}
					}
					else{ //less muatation than possible
						int pos_of_mut = gsl_rng_uniform_int(r, tsize + 1);
						for(int until=0; until < ins.length() ; until++){
							if(pos_of_mut == ins[until]){ // if this mutation position has been before
								pos_of_mut = gsl_rng_uniform_int(r, tsize + 1); //new position
								until = 0; //restart checking
							}
						}
						ins.push_back(pos_of_mut);
					}
*/				}

				//repl
				//seq.clear(); //in theory no need for it...
				
				for(std::string::reverse_iterator old_it = templ.seq.rbegin(); old_it != templ.seq.rend(); old_it++){
					if( gsl_rng_uniform(r) < par_insertion ) seq.push_back( bases[gsl_rng_uniform_int(r, 3)] ); //if there is an insertion add random base
					if( gsl_rng_uniform(r) > par_deletion ) { //if there is no deletion copy template
						if( gsl_rng_uniform(r) < par_substitution ) seq.push_back( bases[( RNAc2i( RNAc2cc( (char) *old_it) ) + gsl_rng_uniform_int(r, 2) + 1) % 4] ); //wrong (substitution)
						else seq.push_back( RNAc2cc( (char) *old_it) ); //good (correct copying)
					}
				}
				if( gsl_rng_uniform(r) < par_insertion ) seq.push_back(something); //if there is an insertion add random base to its end



/*
				for(pos_original=length-1, pos_copy=0; pos_original >= 0; pos_original--){
					//is there a deletion
					if(genrand_real2() < par_deletion) {
						if(genrand_real2() < par_insertion) { //deletion and insertion
							if(pos_copy >= MAXSTRING) {printf("ERROR (nonPerfectReplication): reached max length!\n"); break;} //check if copy is too long
							copy[pos_copy++] = RNAi2c( (int) floor(genrand_real2()*4.0) );
						}
						else {} //length decreases: only deletion
					}
					else {
						if(genrand_real2() < par_insertion) { //length increases: only insertion...
							if(pos_copy + 1 >= MAXSTRING) {printf("ERROR (nonPerfectReplication): reached max length!!\n"); break;} //check if copy is too long
							if(genrand_real2() < 0.5) { // ...to the right
								copy[pos_copy++] = RNAc2cc(original[pos_original]);

								copy[pos_copy++] = RNAi2c( (int) floor(genrand_real2()*4.0) );
							}
							else { // ...to the left
								copy[pos_copy++] = RNAi2c( (int) floor(genrand_real2()*4.0) );
								copy[pos_copy++] = RNAc2cc(original[pos_original]);

							}
						} else { //basic copying
							if(pos_copy >= MAXSTRING) {printf("ERROR (nonPerfectReplication): reached max length!!!\n"); break;} //check if copy is too long
							copy[pos_copy++] = RNAc2cc(original[pos_original]);
						}
					}
				}
				copy[pos_copy] = '\0';
*/
			}

			double* geta(int no){
				if (annot_level < 2){
					if (annot_level) annotate2();
					else return NULL;
				}
				
				return a[no];
			}

			double* geta(){
				if (annot_level < 2){
					if (annot_level) annotate2();
					else return NULL;
				}
				
				return a;
			}

			double getR(){
				if(annot_level < 3) { 
					if (annot_level < 2){
						if (annot_level) annotate2();
						else return -1.0 ;
					}
					annotate3();
				}

				return R;

			}

		private:
			char *str;
			float mfe;
			double Pfold;
			int no_sites;
			std::string seq; //sequence of replicator
			double R; // replication rate
			double *a; //enzymatic activities
			//std::vector<int> ins;
			//std::vector<int> subs;
			//std::vector<int> dels;

			int annot_level;

			//FUNCTIONS
			void annotate() {
				empty = false;
				annot_level = 1;
				
				// predict Minmum Free Energy and corresponding secondary structure
				mfe = vrna_fold(seq.c_str(), str);
					  
				//calculate Pdeg
				Pdeg = 0.9 - 0.8 * mfe / Eopt[seq.length()];
			} 

			void annotate2() {
				annot_level = 2;

				//calculate Pfold
				Pfold = 1/( 1 - std::exp( -cval[seq.length()] * mfe) );
				
				//annotata
			 	no_sites = patterns.search((char*) seq.c_str(), str, a);

				//compute a from alpha
				for(int act = 0; act < par_noEA; act++) {
					a[act] = Pfold * a[act] / m_sigma[no_sites]; 
				}

			}

			void annotate3(){
				annot_level = 3;
				//calculate R
				//R = g / (b1 + b2*L) * (l + (1 - Pfold)) , where L and Pfold are variables
				//if W(L) = g / (b1 + b2*L) , and W[L] is precalculated (L is int)
				//and ll = l + 1
				//R = W(L) * ( ll - Pfold )
				R = length_dep[ seq.length() ] * (par_ll - Pfold) ;

			}

	};	
}


#endif

