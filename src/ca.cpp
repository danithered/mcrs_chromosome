#include "ca.h"

namespace cadv {
	int grid_init(Cell **plane, int size1=300, int size2=300, Ca_layout layout=square) {
		int size = 0;
		
		if(size1 <=0 || size2 <= 0) {
			size =0;
			return(0);
		} 
		else if(size1 == 1 ) {
			size = size2;
		}
		else if(size2 == 1){
			size == size1;
		}
		else switch (layout) { //the matrix is definitely more than 1D based on sizes
				case square:
				case hex:
					size = size1 * size2;
		}
		
		if(layout == square) {
			*plane = new Cell[size];
			if(! *plane ) {
				std::cerr << "ERROR: cadv::ca_init: plane could not be initialized!" << std::endl;
				return(0);
			}
		}
		
		return(size);
	}

	void CellAut::neighInic(double neigh_tipus, Ca_edge edge) {
	    	int maxDist = 0, x = 0, y = 0, noNei = 0;

		std::vector<int> n_inic_x;
		std::vector<int> n_inic_y;

		//Create neighbourhood definition
		if(neigh_tipus == MARGOLUS_NEIGH) {
		    n_inic_x.push_back(0);
		    n_inic_y.push_back(0);

		    n_inic_x.push_back(1);
		    n_inic_y.push_back(0);

		    n_inic_x.push_back(0);
		    n_inic_y.push_back(1);

		    n_inic_x.push_back(1);
		    n_inic_y.push_back(1);
		}

		else{
		    if(CellAut::layout == square) {
			    if(2 <= neigh_tipus ) {
			    	//self
				n_inic_x.push_back(0);
				n_inic_y.push_back(0);
			    
				//other cells
				maxDist = (int) std::log2((int) neigh_tipus - 1);
				for(x = -maxDist; x <= maxDist; x++){ 
					for(y = -maxDist; y <= maxDist; y++){ 
//						std::cout << "std::pow(2, " << x << ") + std::pow(2," << y << ") <= " << neigh_tipus << "\t" << std::pow(2, std::abs(x)) << " " << std::pow(2, std::abs(y)) << std::endl; 
						if( (x || y) && (std::pow(2, std::abs(x)) + std::pow(2, std::abs(y)) <= neigh_tipus ) ) {
							n_inic_x.push_back(x);
							n_inic_y.push_back(y);
						}
					} //end for y
				} //end for x
			    }
		    }
		    else if(layout == hex){
			    if(3 <= neigh_tipus ) {
			    	//self
				n_inic_x.push_back(0);
				n_inic_y.push_back(0);

				//other cells
				maxDist = (int) std::log2((int) neigh_tipus - 2);
				for(x=-maxDist; x <= maxDist; x++){ 
					for(y=-maxDist; y <= maxDist; y++){ 
						if( (x || y) && (std::pow(2, std::abs(x)) + std::pow(2, std::abs(y)) + std::pow(2, std::abs(0-x-y)) <= neigh_tipus ) ) {
							n_inic_x.push_back(x);
							n_inic_y.push_back(y);
						}
					} //end for y
				} //end for x
			    }
			    
		    }
		}

		//iterate through grid
		if(edge == torus){
			if(layout == square){
//				std::cout << "ncol " << ncol << " nrow " << nrow << std::endl;
			    for(int i=0; i < cadv::CellAut::size; i++){ //iterate throught grid
				    matrix[i].no_neigh = n_inic_x.size();
				    matrix[i].neigh = new int[matrix[i].no_neigh] ; //need to rewrite: pointers, not ints!!!
				    for(int n = 0; n < matrix[i].no_neigh; n++) {
					    matrix[i].neigh[n] = ( dvtools::Rmod( ((int)i/ncol + n_inic_y[n]) , nrow) ) * ncol + dvtools::Rmod( dvtools::Rmod(i , ncol) + n_inic_x[n] , ncol);
/**/					    std::cout << "for cell " << i << " the x" << n_inic_x[n] << " y" << n_inic_y[n] << " neighbour is " << matrix[i].neigh[n] << "\t" << (int)i/ncol << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) * ncol << " " << ( i % ncol + n_inic_x[n] ) % ncol << std::endl;
				    } 
			    } //end itarate thru grid
			}
			else if(layout == hex){
			    for(int i=0; i < size; i++){ //iterate throught grid
				    matrix[i].no_neigh = n_inic_x.size();
				    matrix[i].neigh = new int[matrix[i].no_neigh] ; //need to rewrite: pointers, not ints!!!
				    for(int n = 0; n < matrix[i].no_neigh; n++) {
					    matrix[i].neigh[n] = ( ((int)i/ncol + n_inic_y[n]) % nrow ) * ncol + ( i % ncol + n_inic_x[n] + ( n_inic_y[n] + ( (int)i / ncol)&1  )/2 ) % ncol; 
				    } 
			    } //end itarate thru grid
			}

		}
		else if (edge == wall){
			for(int i=0; i < size; i++){ //iterate throught grid
				//count number of neighbours	
				x = (int) i / ncol; 
				y = i % ncol;
				noNei = 0;
				for(int n = 0; n < matrix[i].no_neigh; n++) {
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) noNei++;
				}
				matrix[i].no_neigh = noNei;
				matrix[i].neigh = new int[noNei] ; //need to rewrite: pointers, not ints!!!
				//assign neighbours
				for(int n = 0; n < matrix[i].no_neigh; n++) {	
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) {
						matrix[i].neigh[n] = i + n_inic_x[n] * ncol + n_inic_y[n]; 
						
					}
				}
			}
		}
		else if (edge == mirror){ //does not work!!!
				
			for(int i=0; i < size; i++){ //iterate throught grid
				matrix[i].no_neigh = n_inic_x.size();
				matrix[i].neigh = new int[matrix[i].no_neigh] ; //need to rewrite: pointers, not ints!!!
				for(int n = 0; n < matrix[i].no_neigh; n++) {
				}
			}
		}
		
		
	} //end neighInic
	
}

