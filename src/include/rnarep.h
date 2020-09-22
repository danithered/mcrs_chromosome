#ifndef _RNAREP_
#define _RNAREP_

#include <string.h>
#include <vector>

extern "C" {
#include <ViennaRNA/fold.h>
#include <ViennaRNA/utils/basic.h>
}

#include "randomgen.h"
#include "parameters.h"

namespace rnarep {
	extern char bases[6];

	class CellContent{
		public:
			double value1; //temporary, just for testing

			std::string seq; //sequence of replicator
			double k; // replication rate
			std::vector<double> E; //enzymatic activities
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

			}

			CellContent(std::string input_str){
				seq = input_str;
				if(seq.length()){
					empty = false;
				}
				else {
					empty = true;
				}
			}

			~CellContent(){
				//delete [] (seq);
			}

			//FUNCTIONS
			void annotate() {
				// allocate memory for MFE structure (length + 1)
				char *structure = (char *) vrna_alloc(sizeof(char) * (seq.length() + 1));
				
				// predict Minmum Free Energy and corresponding secondary structure
				float mfe = vrna_fold(seq.data(), structure);
					  
				// print sequence, structure and MFE
				std::cout << seq << std::endl;
				std::cout << structure << mfe << std::endl;

				//cleanup memory
				free(structure);
				
									
			} 
	};	
}


#endif

