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

namespace rnarep {
	extern char bases[6];

	double EoptCalc(int n);
	double cvalCalc(int n);
	double length_depCalc(int n);

	class CellContent{
		public:
			double value1; //temporary, just for testing

			std::string seq; //sequence of replicator
			double Pdeg;
			double R; // replication rate
			double *a; //enzymatic activities

			static dvtools::quickPosVals Eopt;
			static dvtools::quickPosVals cval;
			static dvtools::quickPosVals length_dep;


			int fold; // which fold is it currently
			bool empty; // is this cell empty or not

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
					empty = false;
				}
				else { //empty
					empty = true;
				}

				// allocate memory for MFE structure (length + 1)
				str = (char *) vrna_alloc(sizeof(char) * ( MAXLEN  + 1));

				// allocate memory for enzymaitc activities
				a = new double [par_noEA];

				//inic
				inicVals();
			 }

			CellContent(std::string input_str){
				seq = input_str;
				if(seq.length()){
					empty = false;
				}
				else {
					empty = true;
				}

				// allocate memory for MFE structure (length + 1)
				str = (char *) vrna_alloc(sizeof(char) * ( MAXLEN  + 1));

				// allocate memory for enzymaitc activities
				a = new double [par_noEA];

				//inic
				inicVals();
			}

			~CellContent(){
				//delete [] (seq);
				//cleanup memory
				free(str);
				delete [] (a);
			}

			//FUNCTIONS
			void inicVals(){
				/*Eopt.setFunc(&EoptCalc);
				Eopt.setMax(100);

				cval.setFunc(&cvalCalc);
				cval.setMax(100);
				
				length_dep.setFunc(&length_depCalc);
				length_dep.setMax(100);
				*/
			}

			void annotate() {
				
				// predict Minmum Free Energy and corresponding secondary structure
				mfe = vrna_fold(seq.c_str(), str);
					  
				//calculate Pdeg
				Pdeg = 0.9 - 0.8 * mfe / Eopt[seq.length()];
 
			} 

			void annotate2() {
				//calculate Pfold
				Pfold = 1/( 1 - std::exp( -cval[seq.length()] * mfe) );
				
				//calculate R
				//R = g / (b1 + b2*L) * (l + (1 - Pfold)) , where L and Pfold are variables
				//if W(L) = g / (b1 + b2*L) , and W[L] is precalculated (L is int)
				//and ll = l + 1
				//R = W(L) * ( ll - Pfold )
				R = length_dep[ seq.length() ] * (par_ll - Pfold) ;

				//annotata
				//
			}

		private:
			char *str;
			float mfe;
			double Pfold;
	};	
}


#endif

