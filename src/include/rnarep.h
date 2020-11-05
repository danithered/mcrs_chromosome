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
	extern char bases[6];

	double EoptCalc(int n);
	double cvalCalc(int n);
	double length_depCalc(int n);

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

				// allocate memory for MFE structure (length + 1)
				str = (char *) vrna_alloc(sizeof(char) * ( MAXLEN  + 1));

				// allocate memory for enzymaitc activities
				a = new double [par_noEA];
			 }

			CellContent(std::string input_str){
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

