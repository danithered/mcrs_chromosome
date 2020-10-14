#ifndef _CADV_
#define _CADV_

#include <vector>
#include "randomgen.h"
#include "dv_tools.h"
#include "rnarep.h"
#include <cmath>
#include <iostream>
#include <cstring>

#define SINGLESQ 2.0
#define SINGLEHEX 3.0
#define MOORE_NEIGH 4.0
#define VON_NEUMANN_NEIGH 3.0
#define MARGOLUS_NEIGH -1.0
#define NEIGH5X5 8.0
#define HEX1 4.0

namespace cadv {
	enum Ca_layout {empty=0, single=1, line=2, hex=6, square = 4};
	enum Ca_edge {wall = 0, mirror = 1, torus=2};


/*	class CellContent{
		public:
			double value1; //temporary, just for testing

			char *seq; //sequence of replicator
			double k; // replication rate
			std::vector<double> E; //enzymatic activities
			int fold; // which fold is it currently
			int length; //length of the seq
			bool empty; // is this cell empty or not

			//initialiser
			CellContent(){	
				//initialise array for seq
				if ( !(seq = new char [100]) ) std::cerr << "ERROR: could not allocate space for seq" << std::endl;
				std::memset(seq, '\0', 100);

				//add random values for seq

			}

			~CellContent(){
				delete [] (seq);
			}

	};
*/
	class Cell{
		public:
			//Cell content
			class rnarep::CellContent *vals;

			//Cell topology
			int no_met_neigh;
			int no_repl_neigh;
			int no_diff_neigh;
			
			class Cell **met_neigh;
			class Cell **repl_neigh;
			class Cell **diff_neigh;
			
			/*
			remember: first pointer in neighbourhood array is always "self". So the first pinter to a different cell is neigh[1] 
			*/

			class CellAut *parent;

			//Functions
			int who(Cell * ref){
				return( int (this - ref) );
			}

			void printN(Cell * ref){
				/*for(int p = 0; p < no_neigh; p++){
				    std::cout << "\t" << neigh[p];
				}
				std::cout << std::endl;*/

				std::cout << "I am number " << who(ref) << " and my 1st neighbour is number " << met_neigh[1]->who(ref) << std::endl;

				return;
			}


			//Cell base functions
			Cell(){
				vals = new rnarep::CellContent; //creating place for cell content values

				no_met_neigh = no_repl_neigh = no_diff_neigh = 0; //no inic

				vals->value1 = 1; //TEMPORARY!!
			}

			~Cell(){
				if (no_met_neigh) delete [] (met_neigh);
				if (no_diff_neigh) delete [] (diff_neigh);
				if (no_repl_neigh) delete [] (repl_neigh);

				delete vals;
			}

			///inic neighbours
			void inicNeigh(int n, int type = 1){
				switch(type){
					case 0:
						no_diff_neigh = n;
						diff_neigh = new Cell* [n] ; 
						break;
					case 1:
						no_met_neigh = n;
						met_neigh = new Cell* [n] ; 
						break;
					case 2:
						no_repl_neigh = n;
						repl_neigh = new Cell* [n] ; 
						break;
				}
			}

			///set neighbours
			void setNeigh(class Cell *np, int which_neigh, int type = 1){
				switch(type){
					case 0:
						diff_neigh[which_neigh] = np;
						break;
					case 1:
						met_neigh[which_neigh] = np;
						break;
					case 2:
						repl_neigh[which_neigh] = np;
						break;
				}
			}

			///copies over its value to another
			//void operator >( Cell& target){
			//	target.vals = vals;
			//	return;
			//}

			///diffusion
			void diff(){ // ONE Toffoli-Margoulus step
				static class rnarep::CellContent *temp_vals;

				temp_vals = vals; // 0 -> S
				if(gsl_rng_uniform(r) < 0.5) { //counterclockwise
					vals = diff_neigh[1]->vals; // 1 -> 0
					diff_neigh[1]->vals = diff_neigh[3]->vals; // 3 -> 1
					diff_neigh[3]->vals = diff_neigh[2]->vals; // 2 -> 3
					diff_neigh[2]->vals = temp_vals; // S -> 2

				} else { //cloclwise
					vals = diff_neigh[2]->vals; // 2 -> 0
					diff_neigh[2]->vals = diff_neigh[3]->vals; // 3 -> 2
					diff_neigh[3]->vals = diff_neigh[1]->vals; // 1 -> 3
					diff_neigh[1]->vals = temp_vals; // S -> 1
				}
			}

			double M(){
				for(int met = 0; met < no_met_neigh; met++){
					// M(x) = prod(sum (a_i))
					;
				}
			}

			int update(){
				if (vals.empty) {
					//REPLICATION
					for(int rep = 1; rep < no_repl_neigh; rep++) { // 0. neighbour is self, but it is empty
						if(!repl_neigh[rep]->vals.empty){
							repl_neigh[rep]->vals.R() * repl_neigh[rep]->M();
							;
						}
					}
				}
				else {
					//DEGRADATION
					if(vals.Pdeg < gsl_rng_uniform(r) ) vals.die();
				}
			}
	
	};

	
	class CellAut {
		public:
			int nrow;
			int ncol;
			int size;
			int time;
			cadv::Ca_layout layout;
			
			Cell *matrix;
			
			double diff;
			double saving_freq;

			//FUNCTIONS

			int grid_init() ;

			//Constructor 1
			CellAut(int size1=300, int size2=300, cadv::Ca_layout layout_type=square){
				time=0;
				diff = 0;
				saving_freq = 0;
				nrow=size1;
				ncol=size2;
				layout = layout_type;

				grid_init();

				if(!size) layout = empty;
				if(size1==1 || size2==1){
					if(size1==size2) {
						layout = single;
					}
					else {
						layout = line;
					}
				}
				
//				std::cout << "Basic Constructor Called" << std::endl;
			}
			
			//Constructor 2
			CellAut(int size1, int size2, cadv::Ca_layout layout_type, Cell* pool, double* probs, int no_choices){
				CellAut(size1, size2, layout_type);
				init(pool, probs, no_choices);
			}
			
			//Deconstructor
			~CellAut(){
				if(size) delete [] (matrix);
				//for(int i =0; i < neighbourhoods.size(); i++) {
				//	delete [] (neighbourhoods.at(i));
				//}
//				std::cout << "Deconstructor Called" << std::endl;
			}
			
			//virtual void Update(int cell);
			
			//Functions
			
			
			//Positions
			
			///gives back coordinates of nth cell
			std::vector<int> getCoord(int n); 
			
			///gives back coordinates of nth cell
			std::vector<int> getCoord(int n, int type ); 

			///finds cell in pos [x,y] and gives back a pointer to it
			inline Cell* get(int x, int y) {
				if(layout == square){
					return(matrix + y*ncol + x);
				}
				return(NULL);
			}
			///finds cell in pos [x,y] and gives back its number
			inline int getN(int x, int y) {
				if(layout == square){
					return(y*ncol + x);
				}
				else if (layout == hex){
					return(y + x*ncol + x/2);
				}
				return(-1);
			}

			///gives back pointer to cell
			inline Cell* get(int cell) {
				return(matrix + cell);
			}
			
			///initialises matrix with predefined values, randomly
			void init(Cell* pool, double* probs, int no_choices); 
			
			//Updates

			///One update step - DOES NOT WORK
			int updateStep(int cell){
				matrix[cell].printN(matrix);
				return(0);
			}

			///Random update
			int rUpdate(int gens);

			//Update according to a random order (in every generation all cells will be updated)
			int oUpdate(int gens);

			//Diffusions
			///Random walk step
			
			///Random walk
			
			///Flipping step
			
			///Flipping
			
			///TM inic
			
			///TM step
			
			///TM async
			
			///TM async updorder
			
			///TM classic

			///Initialise neighbourhood
			void neighInic(double neigh_tipus, Ca_edge edge, int neigh_no);



	};
	
}


#endif
