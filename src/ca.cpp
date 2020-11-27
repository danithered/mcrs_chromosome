#include "ca.h"

namespace cadv {
	//FUNCTIONS FOR Cell
	void Cell::inicNeigh(int n, int type){
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
				claims = new double [n+1];
				claims[0] = par_claimEmpty;
				break;
		}
	}

	void Cell::setNeigh(class Cell *np, int which_neigh, int type){
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

	void Cell::diff(){ // ONE Toffoli-Margoulus step
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

	double Cell::M(){
		double M = 1, akt = 0;

		for(int a = 0; a < par_noEA; a++){
			akt = 0;
			for(int met = 0; met < no_met_neigh; met++){
				// M(x) = prod(sum (a_i))
				akt += met_neigh[met]->vals->geta(a) ;
			}
			M *= akt;
		}

		return M;
	}

	void Cell::update(){
		double sum = 0.0;
		int decision = 0;

		if (vals->empty) { // if focal cell is empty (it can be filled with a copy)
			//REPLICATION
			for(int rep = 1; rep < no_repl_neigh; rep++) { // 0. neighbour is self, but it is empty
				if(!repl_neigh[rep]->vals->empty){
					sum += (claims[rep] = repl_neigh[rep]->vals->getR() * repl_neigh[rep]->M() );
				}
				else claims[rep] = 0.0;
			}
			//decision
			decision = dvtools::brokenStickVals(claims, no_repl_neigh + 1, sum, gsl_rng_uniform(r)) ;
			if(decision){ //claim 0 is claimEmpty NOTE that the probablity of staying empty is not fixed (e.g. 10%)!
				vals->replicate( *(repl_neigh[decision]->vals) );
			}
		}
		else { //if focal cell is occupied (it can die)
			//DEGRADATION
			if(vals->Pdeg < gsl_rng_uniform(r) ) vals->die();
		}
	}





	//FUNCTIONS FOR CellAut
	int CellAut::grid_init() {
		size = 0;
		
		//calculating sizes
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
		
		//initialising grid
		if(layout == square || layout == hex) {
			matrix = new Cell[size];
			if(! matrix ) {
				std::cerr << "ERROR: cadv::ca_init: matrix could not be initialized!" << std::endl;
				return(0);
			}
		}
//		std::cout << "grid_init: ncol: " << ncol << ", nrow: " << nrow << ", size " << size << std::endl;
		
		//setting parent
		for(int i=0; i < size; i++){
			matrix[i].parent = this;
		}

		return(size);
	}

	///gives back coordinates of nth cell
	std::vector<int> CellAut::getCoord(int n) {
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

	///initialises matrix with predefined values, randomly
	void CellAut::init(Cell* pool, double* probs, int no_choices) {
		int i = 0;
		double sum = 0.0;
		
		for(i=0; i < no_choices; i++) {
			sum += probs[i];
		}
		for(i=0; i < size; i++) {
			matrix[i] = pool[dvtools::brokenStickVals(probs, no_choices, sum, gsl_rng_uniform(r) )];
		}
	}	


	inline Cell* CellAut::get(int x, int y) {
		if(layout == square){
			return(matrix + y*ncol + x);
		}
		return(NULL);
	}
	///finds cell in pos [x,y] and gives back its number
	inline int CellAut::getN(int x, int y) {
		if(layout == square){
			return(y*ncol + x);
		}
		else if (layout == hex){
			return(y + x*ncol + x/2);
		}
		return(-1);
	}

	///gives back pointer to cell
	inline Cell* CellAut::get(int cell) {
		return(matrix + cell);
	}

	//a singel update step <- this is called by rUpdate and oUpdate
	int CellAut::updateStep(int cell){
		matrix[cell].printN(matrix);
		return(0);
	}

	///Random update
	int CellAut::rUpdate(int gens){
		int iter=0, diff_until = dvtools::fracpart(diff * size * time);

		for(int mtime = time + gens ; time < mtime ; time++){ //updating generations
			for(iter = 0; iter < size; iter++){
				//UPDATING
				updateStep( gsl_rng_uniform_int(r, size) );
				//DIFFUSION
				for(diff_until += diff; diff_until >= 1; diff_until--){
					matrix[ gsl_rng_uniform_int(r, size) ].diff();
				}
			}
		}
		return(0);
	}

	//Update according to a random order (in every generation all cells will be updated)
	int CellAut::oUpdate(int gens){
		int *order;
		int iter=0, temp = 0, target = 0, diff_until = dvtools::fracpart(diff * time);

		order = new int[size];
		for(iter = 0; iter < size; iter++){
			order[iter] = iter;
		}

		for(int mtime = time + gens ; time < mtime ; time++){ //updating generations
			//UPDATING
			for(iter = 0; iter < size; iter++){
				target = gsl_rng_uniform_int(r, size - iter);
				if (target) {
					target += iter;
					temp = order[target];
					order[ target ] = order[ iter ];
					order[ iter ] = temp;
					updateStep( temp );
				}
				else {
					updateStep( order[iter] );
				}
			}
			//DIFFUSION
			for(diff_until += diff; diff_until >= 1; diff_until--){
				for(iter = 0; iter < size; iter++){
					target = gsl_rng_uniform_int(r, size - iter);
					if (target) {
						target += iter;
						temp = order[target];
						order[ target ] = order[ iter ];
						order[ iter ] = temp;
						matrix[ temp ].diff();
					}
					else {
						matrix[ order[iter] ].diff();
					}
				}
			}
		}

		delete [] (order);

		return(0);
	}

	//gives back coordinates of nth cell
	std::vector<int> CellAut::getCoord(int n, int type = 0) {
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

	void CellAut::neighInic(double neigh_tipus, Ca_edge edge, int neigh_no = 1) {
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
				    matrix[i].inicNeigh( n_inic_x.size(), neigh_no );
				    //matrix[i].no_neigh = n_inic_x.size();
				    //matrix[i].neigh = new int[matrix[i].no_neigh] ; 
				    //matrix[i].neigh = new Cell* [matrix[i].no_neigh] ; 
				    for(int n = 0; n < n_inic_x.size(); n++) {
					    //matrix[i].neigh[n] = matrix + ( ( dvtools::Rmod( ((int)i/ncol + n_inic_y[n]) , nrow) ) * ncol + dvtools::Rmod( dvtools::Rmod(i , ncol) + n_inic_x[n] , ncol));
					    matrix[i].setNeigh( matrix + ( ( dvtools::Rmod( ((int)i/ncol + n_inic_y[n]) , nrow) ) * ncol + dvtools::Rmod( dvtools::Rmod(i , ncol) + n_inic_x[n] , ncol)), n, neigh_no );
//					    std::cout << "for cell " << i << " the x" << n_inic_x[n] << " y" << n_inic_y[n] << " neighbour is " << matrix[i].neigh[n] << "\t" << (int)i/ncol << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) << " " << ( ((int)i/ncol + n_inic_y[n]) % nrow ) * ncol << " " << ( i % ncol + n_inic_x[n] ) % ncol << std::endl;
				    } 
			    } //end itarate thru grid
			}
			else if(layout == hex){
			    for(int i=0; i < size; i++){ //iterate throught grid
				    matrix[i].inicNeigh( n_inic_x.size(), neigh_no );
				    //matrix[i].no_neigh = n_inic_x.size();
				    //matrix[i].neigh = new Cell*[matrix[i].no_neigh] ;
				    for(int n = 0; n < n_inic_x.size(); n++) {
					    //matrix[i].neigh[n] = matrix + ( ( dvtools::Rmod((int)i/ncol + n_inic_y[n] , nrow ) ) * ncol + dvtools::Rmod ( dvtools::Rmod( i, ncol) + n_inic_x[n] + ( n_inic_y[n] + ( (int)i / ncol)&1  )/2 , ncol)) ; 
					    matrix[i].setNeigh( matrix + ( ( dvtools::Rmod((int)i/ncol + n_inic_y[n] , nrow ) ) * ncol + dvtools::Rmod ( dvtools::Rmod( i, ncol) + n_inic_x[n] + ( n_inic_y[n] + ( (int)i / ncol)&1  )/2 , ncol)), n, neigh_no ); 
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
				for(int n = 0; n < n_inic_x.size(); n++) {
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) noNei++;
				}
				matrix[i].inicNeigh(noNei, neigh_no);
				//matrix[i].no_neigh = noNei;
				//matrix[i].neigh = new Cell* [noNei] ; 
				
				//assign neighbours
				for(int n = 0; n < noNei; n++) {	
					if(x + n_inic_x[n] >= 0 && x + n_inic_x[n] < ncol && y + n_inic_y[n] >= 0 && y + n_inic_y[n] < nrow  ) {
						//matrix[i].neigh[n] = matrix + (i + n_inic_x[n] * ncol + n_inic_y[n]); 
						matrix[i].setNeigh( matrix + (i + n_inic_x[n] * ncol + n_inic_y[n]), n, neigh_no ); 
						
					}
				}
			}
		}
		else if (edge == mirror){ //does not work!!!
				
			for(int i=0; i < size; i++){ //iterate throught grid
				matrix[i].inicNeigh(n_inic_x.size(), neigh_no);
				//matrix[i].no_neigh = n_inic_x.size();
				//matrix[i].neigh = new Cell* [matrix[i].no_neigh] ;
				for(int n = 0; n < n_inic_x.size(); n++) {
				}
			}
		}
		
		
	} //end neighInic
	
}

