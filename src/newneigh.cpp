#include <iostream>
#include <cmath>
#include "ca.h"

/*be vector n_inic_x and n_inic_y int vectors in the class*/

namespace cadv {
	int* neighInic(Ca_layout layout, double neigh_tipus, Ca_edge edge) {
	    	int maxDist = 0, x = 0, y = 0, noNei = 0;

		std::vector<int> n_inic_x;
		std::vector<int> n_inic_y;

		//Create neighbourhood definition
		if(layout == square) {
			maxDist = std::log2(1- (int) neigh_tipus);
			for(x = -maxDist; x <= maxDist; x++){ 
				for(y = -maxDist; y <= maxDist; y++){ 
					if(std::pow(2, x) + std::pow(2,y) <= neigh_tipus ) {
						n_inic_x.push_back(x);
						n_inic_y.push_back(y);
					}
				} //end for y
			} //end for x
		}
		else if(layout == hex){
			maxDist = std::log2(2- (int) neigh_tipus);
			for(x=0; x <= maxDist; x++){ 
				for(y=0; y <= maxDist; y++){ 
					if(std::pow(2, x) + std::pow(2,y) + std::pow(2, 0-x-y) <= neigh_tipus ) {
						n_inic_x.push_back(x);
						n_inic_y.push_back(y);
					}
				} //end for y
			} //end for x
			
		}

		//iterate through grid
		if(edge == torus){
			for(int i=0; i < size; i++){ //iterate throught grid
				matrix[i]->no_neigh = n_inic_x.size();
				matrix[i]->neigh = new int[matrix[i]->no_neigh] ; //need to rewrite: pointers, not ints!!!
				for(int n = 0; n < matrix[i]->no_neigh; n++) {
					matrix[i]->neigh[n] = ( ((int)i/ncol + n_inic_y[n]) % nrow ) * ncol + ( i % ncol + n_inic_x[n] ) % ncol;
				} 
			} //end itarate thru grid
		}
		else if (edge == wall){
			for(int i=0; i < size; i++){ //iterate throught grid
				//count number of neighbours	
				x = (int) i / ncol; 
				y = i % ncol;
				noNei = 0;
				for(int n = 0; n < matrix[i]->no_neigh; n++) {
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) noNei++;
				}
				matrix[i]->no_neigh = noNei;
				matrix[i]->neigh = new int[noNei] ; //need to rewrite: pointers, not ints!!!
				//assign neighbours
				for(int n = 0; n < matrix[i]->no_neigh; n++) {	
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) {
						matrix[i]->neigh[n] = i + n_inic_x[n] * ncol + n_inic_y[n]; 
						
					}
				}
			}
		}
		else if (edge == mirror){ //does not work!!!
				
			for(int i=0; i < size; i++){ //iterate throught grid
				matrix[i]->no_neigh = n_inic_x.size();
				matrix[i]->neigh = new int[matrix[i]->no_neigh] ; //need to rewrite: pointers, not ints!!!
				for(int n = 0; n < matrix[i]->no_neigh; n++) {
				}
			}
		}
		
		
	} //end neighInic
	
}

