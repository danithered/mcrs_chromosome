#ifndef _SEQANNOT_
#define _SEQANNOT_

#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "parameters.h"

namespace dv_annot{

	class Subrule {
		public:
			double *value; //the value of the structure
			int no_bases; //number of bases as subrule

			char *base;
			int *pos;

			//Constructor
			Subrule(){
				no_bases = 0;
				value = new double [par_noEA];
/**/				std::cout << "Subrule initialised" << std::endl;
			}

			//Destructor
			~Subrule(){
/**/				std::cout << "Subrule destructor called" << std::endl;
				delete [] (value);

				if (no_bases) {
					delete [] (base);
					delete [] (pos);
				}
/**/				std::cout << "Subrule destructor ended" << std::endl;
			}

			//Funcion to initialise base
			//initBases(int _no_bases) {
			//	no_bases = _no_bases;
			//	base = new char[no_bases];
			//	pos = new int[no_bases];
			//}

			//Function to add all the bases at once
			void addBases(int _no_bases, char *_base, int *_pos) {
				no_bases = _no_bases;
				base = new char[no_bases];
				pos = new int[no_bases];
				std::strncpy(base, _base, no_bases);
				std::memcpy(pos, _pos, sizeof(int) * no_bases);
			}

			//Function to add base
			//void setBase (int number, int pos, char base){
			//	pos[number] = pos;
			//	base[number] = base;
			//}

	}; //Subrule

	class Rule {
		public:
			int pattern_length;
			char *pattern;

			int no_subrules;
			class Subrule *subrules;

			//Constructor
			Rule(int _no_subrules) : no_subrules{_no_subrules} {
/**/				std::cout << "Rule init started" << std::endl;
				subrules = new class Subrule [no_subrules];
				pattern_length = 0;
/**/				std::cout << "Initialised Rule" << std::endl;
			}

			//Destructor
			~Rule(){
/**/				std::cout << "Rule destructor called" << std::endl;
				delete [] (subrules);
				if (pattern_length) delete [] (pattern);
/**/				std::cout << "Rule destructor ended" << std::endl;
			}


			//Functions
			void setPattern(std::string p){
				pattern_length = p.length();
				if(pattern_length){
					pattern = new char [pattern_length + 1];
					std::strcpy(pattern , p.c_str());
					//pattern[pattern_length] = '\0';
				}
/**/				std::cout << "Rule pattern set" << std::endl;
			}

			void setSubrules(std::ifstream &file){
				std::string line;
				std::vector<char> chars;
				std::vector<int> positions;
				char c;
				int p = -1, val = 0;

				for( int sr=0; sr < no_subrules; sr++){
					std::getline(file, line);
					std::istringstream linestream(line);
					chars.empty();
					positions.empty();

					while(linestream >> p) { //read in position
						linestream >> c; //read in character
						chars.push_back(c);
						positions.push_back(p);
					}
					subrules[sr].addBases( chars.size(), chars.data(), positions.data()); //.data() is only cpp11, replace with &x[0] 
					//adding values
					for(int eaval = 0; eaval < par_noEA; eaval++) file >> subrules[sr].value[eaval];
				}
/**/				std::cout << "Rule: subrule added" << std::endl;
			}

	}; //Rule

	class PatternPool {
		public:
			//extern int par_noEA;
			std::vector<class Rule> rules;	//pointer for rules
			double *a;		//enzim activities for a search

			//Constructor
			PatternPool(){
/**/				std::cout << "Pattern pool init started" << std::endl;
				if (par_noEA) a = new double [par_noEA];
				for(int i=0; i < par_noEA; i++) a[i] = 0.0;
				//std::memset(a, 0, sizeof(double) * par_noEA); //remember: use memset only for 0 in case of int!!
/**/				std::cout << "Pattern pool init ended" << std::endl;
			}

			//Destructor
			~PatternPool(){
/**/				std::cout << "PatternPool destructor called" << std::endl;
				if (par_noEA) delete [] (a);
/**/				std::cout << "PatternPool destructor ended" << std::endl;
			}

			//file input
			void readFile(char *filename){
				int no = 0; //number of ea, subrules
				std::string word;
				
				std::ifstream file;
				file.open(filename);

				//read first word which is number of enzymatic activities
				file >> no;
				if( no != par_noEA){
					if (par_noEA) delete [] (a);
					par_noEA = no;
					if (par_noEA) a = new double [par_noEA];
					for(int i=0; i < par_noEA; i++) a[i] = 0.0;
				}
				no = 0;

				//read in rest of file
				while (file >> no){ //read number of subrules
					rules.push_back(Rule(no)); //created Rule
					file >> word; //read pattern
					rules.back().setPattern(word);
					rules.back().setSubrules(file); //set subrules (all)
				}

				//closing file
				file.close();

			}
			
			//clearing activities
			void clear(){
				if(a) for(int i=0; i < par_noEA; i++) a[i] = 0.0;
			}

			//searching pattern
			double * search(char *str){
				//std::strstr();
				return(a);
			}

	}; //PatternPool

}

#endif

