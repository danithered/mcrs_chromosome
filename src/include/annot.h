#ifndef _SEQANNOT_
#define _SEQANNOT_

#include <fstream>

namespace dv_annot{

	class Subrule {
		public:
			double value; //the value of the structure
			int no_bases; //number of bases as subrule

			char *base;
			int *pos;

			//Constructor
			Subrule(){
				no_bases = 0;
			}

			//Destructor
			~Subrule(){
				if (no_bases) {
					delete [] (base);
					delete [] (pos);
				}
			}

			//Funcion to initialise base
			initBases(int _no_bases) {
				no_bases = _no_bases;
				base = new char[no_bases];
				pos = new int[no_bases];
			}

			//Function to add all the bases at once
			addBases(int _no_bases, char *_base, int *_pos) : no_bases{_no_bases}, base{_base}, pos{_pos} {}

			//Function to add base
			void setBase (int number, int pos, char base){
				pos[number] = pos;
				base[number] = base;
			}

	} //Subrule

	class Rule {
		public:
			int pattern_length;
			char *pattern;

			int no_subrules;
			class Subrule *subrules;

			//Constructor
			Rule(int _no_subrules) : no_subrules{_no_subrules} {
				subrules = new class Subrule [no_subrules];
				pattern_length = 0;
			}

			//Destructor
			~Rule(){
				delete [] (subrules);
				if (pattern_length) delete [] (pattern);
			}


			//Functions
			void setSubrule(char * vmi){
				;
			}

	} //Rule

	class PatternPool {
		public:
			;
	} //PatternPool

}

#endif

