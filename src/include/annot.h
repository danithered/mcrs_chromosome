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
//				std::cout << "Subrule initialised" << std::endl;
			}

			//Copy Constructor
			Subrule(const Subrule &obj){
				int i = 0;
				no_bases = obj.no_bases;
				if(no_bases){
					base = new char[no_bases];
					pos = new int[no_bases];
					for(i = 0; i < no_bases; i++){
						base[i] = obj.base[i];
						pos[i] = obj.pos[i];
					}
				}
				value = new double [par_noEA];
				for(i = 0; i < par_noEA; i++){
					value[i] = obj.value[i];
				}
//				std::cout << "Subrule copied" << std::endl;
			}

			//Operator =
			void operator = (const Subrule &obj){
				int i = 0;
				no_bases = obj.no_bases;
				if(no_bases){
					base = new char[no_bases];
					pos = new int[no_bases];
					for(i = 0; i < no_bases; i++){
						base[i] = obj.base[i];
						pos[i] = obj.pos[i];
					}
				}
				value = new double [par_noEA];
				for(i = 0; i < par_noEA; i++){
					value[i] = obj.value[i];
				}
//				std::cout << "Subrule copied by = operator" << std::endl;
			}

			//Destructor
			~Subrule(){
//				std::cout << "Subrule destructor called" << std::endl;
				delete [] (value);

				if (no_bases) {
					delete [] (base);
					delete [] (pos);
				}
//				std::cout << "Subrule destructor ended" << std::endl;
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
//				std::cout << "Rule init started" << std::endl;
				if(no_subrules) subrules = new class Subrule [no_subrules];
				pattern_length = 0;
//				std::cout << "Initialised Rule" << std::endl;
			}

			//Copy Constructor
			Rule(const Rule &obj){
//				std::cout << "Rule copy init started" << std::endl;
				pattern_length = obj.pattern_length;
				if(pattern_length){
					pattern = new char [pattern_length + 1];
					std::strcpy(pattern , obj.pattern);
				}

				no_subrules = obj.no_subrules;
				if(no_subrules) {
					subrules = new class Subrule [no_subrules];
					for (int s = 0; s < no_subrules; s++){
						subrules[s] = obj.subrules[s]; 
					}
				
				}
				else {
					subrules = NULL;
				}
//				std::cout << "Rule Copied" << std::endl;
			}

			//Destructor
			~Rule(){
//				std::cout << "Rule (" << no_subrules << ") destructor called" << std::endl;
				if(no_subrules) delete [] (subrules);
//				std::cout << "Rule destructor halfway" << std::endl;
				if (pattern_length) delete [] (pattern);
//				std::cout << "Rule destructor ended" << std::endl;
			}


			//Functions
			void setPattern(std::string p){
				pattern_length = p.length();
				if(pattern_length){
					pattern = new char [pattern_length + 1];
					std::strcpy(pattern , p.c_str());
					//pattern[pattern_length] = '\0';
				}
//				std::cout << "Rule pattern set" << std::endl;
			}

			void setSubrules(std::ifstream &file){
//				std::cout << file.rdbuf();
				std::string line;
				std::vector<char> chars;
				std::vector<int> positions;
				char c;
				int p = -1, val = 0;

				for( int sr=0; sr < no_subrules; sr++){
//					if(! (file) ) std::cout << "File is NULL at sr= " << sr << std::endl;
					std::getline(file, line); //jump to next line
					std::getline(file, line);
					std::istringstream linestream(line);

//					std::cout << "Line read: " << line << std::endl;
					chars.empty();
					positions.empty();

					while(linestream >> p) { //read in position
						linestream >> c; //read in character
						chars.push_back(c);
						positions.push_back(p);
//						std::cout << "new subrule element added: " << c << "at pos " << p << std::endl;
					}
					subrules[sr].addBases( chars.size(), chars.data(), positions.data()); //.data() is only cpp11, replace with &x[0] 
					//adding values
//					if(! (file) ) std::cout << "File is NULL at (2) sr= " << sr << std::endl;
					for(int eaval = 0; eaval < par_noEA; eaval++) {
//						if(! (file) ) std::cout << "File is NULL at (3, eaval= " << eaval << ") sr= " << sr << std::endl;
						file >> subrules[sr].value[eaval];
//						std::cout << "value read (at eaval" << eaval << "):" << subrules[sr].value[eaval] << std::endl;
					}
//					if(! (file) ) std::cout << "File is NULL at (4) sr= " << sr << std::endl;
				}
//				std::cout << "Rule: subrule added" << std::endl;
			}

	}; //Rule

	class PatternPool {
		public:
			//extern int par_noEA;
			std::vector<class Rule> rules;	//pointer for rules
			double *a;		//enzim activities for a search

			//Constructor
			PatternPool(){
//				std::cout << "Pattern pool init started" << std::endl;
				if (par_noEA) a = new double [par_noEA];
				for(int i=0; i < par_noEA; i++) a[i] = 0.0;
				//std::memset(a, 0, sizeof(double) * par_noEA); //remember: use memset only for 0 in case of int!!
//				std::cout << "Pattern pool init ended" << std::endl;
			}

			//Destructor
			~PatternPool(){
//				std::cout << "PatternPool destructor called" << std::endl;
				if (par_noEA) delete [] (a);
				rules.clear();
//				std::cout << "PatternPool destructor ended" << std::endl;
			}

			//file input
			void readFile(char *filename){
				int no = 0, min = 0, order = 0; //number of ea, subrules
				std::string word;
				
				std::ifstream file;
				file.open(filename);
				if(!file.is_open()) std::cerr << "ERROR: file (" << filename << ") not found!" << std::endl;
//				std::cout << file.rdbuf();

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
//					std::cout << "word read: " << word << std::endl;
					rules.back().setPattern(word);
					rules.back().setSubrules(file); //set subrules (all)
				}
//				if(file )std::cout << file.tellg() << " " << file.gcount() << std::endl; else std::cout << "file is NULL" << std::endl; 

				//closing file
				file.close();

				//sorting rules by pattern_length 
				if(rules.size() > 1) {
					class Rule memory(0);
					for( no = 0; no < (rules.size() - 1); no++){
						for(order = no + 1, min = no; order < rules.size(); order++){
							if(rules[order].pattern_length < rules[min].pattern_length ) min = order;
						}
						if(min != no) {
//							std::cout << sizeof(memory) << " " << sizeof(rules[min]) << " " << sizeof(rules[no]) << std::endl;
							/*
							memcpy(&memory, &rules[min], sizeof(rules[min]) ); //save min to memory
							memcpy(&rules[min], &rules[no], sizeof(rules[no]) ); //copy no to min
							memcpy(&rules[no], &memory, sizeof(memory) ); //copy memory to no 
							*/
							memory = rules[min]; //save min to memory
							rules[min] = rules[no]; //copy no to min
							rules[no] = memory; //copy memory to no
						}

					}
					memory.pattern_length = 0; //to ensure no double free will happen
					memory.no_subrules = 0; //detto
				}
		
			}
			
			//clearing activities
			void clear(){
				if(a) for(int i=0; i < par_noEA; i++) a[i] = 0.0;
			}

			//searching pattern
			double * search(char *seq, char *str){
				char *templ, *templ_seq;
				int templ_length = std::strlen(seq);
				
				if(templ_length) for(int search = 0; search < rules.size() && rules[search].pattern_length <= templ_length ; search++){
/**/					std::cout << "search = " << search << std::endl;
					for(templ = std::strstr(str, rules[search].pattern) ; templ != NULL; templ = std::strstr(++templ, rules[search].pattern)){
/**/						std::cout << "looking for structure [" << rules[search].pattern << "] in str (search = " << search << ")"  << std::endl;
						//templ is a pointer to the start of the pattern
						//look for the subrules
						for(int sr = 0, sr_max = rules[search].no_subrules; sr < sr_max; sr++){
/**/							std::cout << "sr = " << sr << std::endl;
							//looking tru the subrules
							char *base = rules[search].subrules[sr].base;
							int *pos = rules[search].subrules[sr].pos;
							int *pos_max = pos + rules[search].subrules[sr].no_bases;

							templ_seq = seq + (templ - str);
							for(; templ_seq[*pos] == *base && pos != pos_max; base++, pos++){}

							if (pos == pos_max){
								for(int v = 0; v < par_noEA; v++){
									for(int act = 0; act < par_noEA; act++) a[act] += rules[search].subrules[sr].value[act];
								}
								break;
							}
						} //sr in subrules
					} // pattern search in sequence
				} // search in rules



				return(a);
			}

	}; //PatternPool

}

#endif

