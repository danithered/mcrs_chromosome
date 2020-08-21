#ifndef _CADV_
#define _CADV_

#include <vector>
#include "randomgen.h"
#include "dv_tools.h"
#include <cmath>
#include <iostream>

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

	class Cell{
		public:
			double value1;
			int no_neigh;
			int *neigh;

			void printN(){
				for(int p = 0; p < no_neigh; p++){
				    std::cout << "\t" << neigh[p];
				}
				std::cout << std::endl;
				return;
			}

			~Cell(){
				delete [] (neigh);
			}
	
	};

	int grid_init(Cell **plane, int size1, int size2, Ca_layout layout) ;
	
	class CellAut {
		public:
			int nrow;
			int ncol;
			int size;
			int time;
			cadv::Ca_layout layout;
			
			Cell *matrix;
			
			//Constructor 1
			CellAut(int size1=300, int size2=300, cadv::Ca_layout layout_type=square){
				time=0;
				nrow=size1;
				ncol=size2;
				
				size = cadv::grid_init(&matrix, size1, size2, layout_type);
				
				if(!size) layout = empty;
				if(size1==1 || size2==1){
					if(size1==size2) {
						layout = single;
					}
					else {
						layout = line;
					}
				}
				
				layout=square;
			
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
			std::vector<int> getCoord(int n) {
				std::vector<int> coords;
				if (layout == square){
					coords.push_back(n % ncol); //x coord
					coords.push_back(n / ncol); //y coord
				}
				else if (layout == hex) {
					//x=n %/% ncol
					//y = n - x*ncol - x%/%2
					//z = 0-x-y
					coords.push_back( n / ncol); //x cubic coord
					coords.push_back( n - coords[0]*ncol - coords[0]/2 ); //y cubic coord
					coords.push_back(0 - coords[0] - coords[1]); //z cubic coord
				}

				return(coords);
			}
			
			///gives back coordinates of nth cell
			std::vector<int> getCoord(int n, int type = 0) {
				std::vector<int> coords;
				if (layout == square){
					coords.push_back(n % ncol); //x coord
					coords.push_back(n / ncol); //y coord
				}
				else if (layout == hex) {
					if(type == 0){ //cube coordinates - axial coordinates are the choosen two from this three coords
						coords.push_back( n / ncol); //x cubic coord
						coords.push_back( n - coords[0]*ncol - coords[0]/2 ); //y cubic coord
						coords.push_back(0 - coords[0] - coords[1]); //z cubic coord
					}
					else if (type == 1){ //axial coords 
						coords.push_back( n / ncol); //x cubic coord
						coords.push_back( n - coords[0]*ncol - coords[0]/2 ); //y cubic coord
					}
					else if (type == 2){ //offset coords 
						coords.push_back(n % ncol); //x coord
						coords.push_back(n / ncol); //y coord
					}
				}

				return(coords);
			}

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
			void init(Cell* pool, double* probs, int no_choices) {
				int i = 0;
				double sum = 0.0;
				
				for(i=0; i < no_choices; i++) {
					sum += probs[i];
				}
				for(i=0; i < size; i++) {
					matrix[i] = pool[dvtools::brokenStickVals(probs, no_choices, sum, gsl_rng_uniform(r) )];
				}
			}
			
			///swaps two cells in the matrix
			
			///copies over a value on an other
			
			//Updates

			///One update step - DOES NOT WORK
			int updateStep(int cell){
				matrix[cell].printN();
				return(0);
			}

			///Random update
			int rUpdate(int gens){
				int iter=0;

				for(int mtime = time + gens ; time < mtime ; time++){ //updating generations
					for(iter = 0; iter < size; iter++){
						updateStep( gsl_rng_uniform_int(r, size) );
					}
				}
				return(0);
			}

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
			void neighInic(double neigh_tipus, Ca_edge edge);

	};
	
}


#endif
