#include "ca.h"

namespace cadv {
	int CellAut::grid_init() {
		size = 0;
		
		if(nrow <=0 || ncol <= 0) {
			size =0;
//			std::cout << "grid_init: size is 0" << std::endl;
			return(0);
		} 
		else if(nrow == 1 ) {
//			std::cout << "grid_init: nrow = 1" << std::endl;
			size = ncol;
		}
		else if(ncol == 1){
//			std::cout << "grid_init: ncol = 1" << std::endl;
			size == nrow;
		}
		else {
//			std::cout << "grid_init: matrix is more than 1D" << std::endl;
			switch (layout) { //the matrix is definitely more than 1D based on sizes
				case square:
				case hex:
					size = nrow * ncol;
			}
		}
//		std::cout << "grid_init: here2: ncol: " << ncol << ", nrow: " << nrow << ", size: " << size << std::endl;
		
		if(layout == square) {
			matrix = new Cell[size];
			if(! matrix ) {
				std::cerr << "ERROR: cadv::ca_init: matrix could not be initialized!" << std::endl;
				return(0);
			}
		}
//		std::cout << "grid_init: ncol: " << ncol << ", nrow: " << nrow << ", size " << size << std::endl;
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
/**/				std::cout << "ncol " << ncol << " nrow " << nrow << std::endl;
			    for(int i=0; i < cadv::CellAut::size; i++){ //iterate throught grid
				    matrix[i].no_neigh = n_inic_x.size();
				    //matrix[i].neigh = new int[matrix[i].no_neigh] ; 
				    matrix[i].neigh = new Cell* [matrix[i].no_neigh] ; 
				    for(int n = 0; n < matrix[i].no_neigh; n++) {
					    matrix[i].neigh[n] = matrix + ( ( dvtools::Rmod( ((int)i/ncol + n_inic_y[n]) , nrow) ) * ncol + dvtools::Rmod( dvtools::Rmod(i , ncol) + n_inic_x[n] , ncol));
/**/					    std::cout << "for cell " << i << " the x" << n_inic_x[n] << " y" << n_inic_y[n] << " neighbour is " << matrix[i].neigh[n] << "\t" << (int)i/ncol << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) * ncol << " " << ( i % ncol + n_inic_x[n] ) % ncol << std::endl;
				    } 
			    } //end itarate thru grid
			}
			else if(layout == hex){
			    for(int i=0; i < size; i++){ //iterate throught grid
				    matrix[i].no_neigh = n_inic_x.size();
				    matrix[i].neigh = new Cell*[matrix[i].no_neigh] ;
				    for(int n = 0; n < matrix[i].no_neigh; n++) {
					    matrix[i].neigh[n] = matrix + ( ( dvtools::Rmod((int)i/ncol + n_inic_y[n] , nrow ) ) * ncol + dvtools::Rmod ( dvtools::Rmod( i, ncol) + n_inic_x[n] + ( n_inic_y[n] + ( (int)i / ncol)&1  )/2 , ncol)) ; 
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
				matrix[i].neigh = new Cell* [noNei] ; 
				//assign neighbours
				for(int n = 0; n < matrix[i].no_neigh; n++) {	
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) {
						matrix[i].neigh[n] = matrix + (i + n_inic_x[n] * ncol + n_inic_y[n]); 
						
					}
				}
			}
		}
		else if (edge == mirror){ //does not work!!!
				
			for(int i=0; i < size; i++){ //iterate throught grid
				matrix[i].no_neigh = n_inic_x.size();
				matrix[i].neigh = new Cell* [matrix[i].no_neigh] ;
				for(int n = 0; n < matrix[i].no_neigh; n++) {
				}
			}
		}
		
		
	} //end neighInic
	
}

