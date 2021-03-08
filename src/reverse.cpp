#include <iostream>
#include "ca.h"
#include "rnarep.h"

using namespace std;

/*argoments:
nrow
ncol
noEA
pattern pool
grid file
*/

int par_insertion, par_deletion, par_substitution;

int main(int argc, char *argv[]) {

	par_insertion = par_deletion = par_substitution = 0;
	int par_nrow = std::atoi(argv[1]), par_ncol = std::atoi(argv[2], par_noEA = std::atoi(argv[3]);

	//create remp cellcontent file
	rnarep::CellContent cell;

	cadv::CellAut automata(par_nrow, par_ncol); //initialise automata
	rnarep::CellContent::patterns.readFile( argv[4] ); //read in pattern file
	automata.init_fromfile( argv[5] ); //load from file

	for(int i=0; i < par_nrow*par_ncol; i++){
		cell.replicate(   automata.get(i)->vals   );
		
		//std::cout << cell.get_seq() << cell.get_str() << 
		if(cell.empty) out << emptystring; 
		else {
			std::cout << *(cell.get_seq())
				<< ' ' << cell.get_str()
				<< ' ' << cell.get_mfe() 
				<< ' ' << cell.getPfold()
				<< ' ' << cell.Pdeg 
				<< ' ' << cell.get_no_sites()
				<< ' ' << cell.getR()
				<< ' ' << cell.M()
				<< ' ' << cell.get_type() ; 
			for(double *a = cell.geta(), *a_until = cell.geta() + par_noEA; a != a_until; a++){
				out << ' ' << *a;
			}

			out << std::endl;
		}
	}


	return 0;
}
