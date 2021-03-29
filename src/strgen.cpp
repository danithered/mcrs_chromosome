#include <iostream>
#include <vector>
#include <string.h>
#include "randomgen.h"

using namespace std;

int main(int argc, char *argv[]) {
	//initialise rng
	time_t timer;
	randomszam_inic( time(&timer) , r);

	int N = gsl_ran_binomial(r, 0.5, 20);

	int n_p = gsl_ran_binomial(r, 0.4, N/2);
	int no_paired = n_p*2;
	std::string str;

	if(no_paired){
		str.assign(no_paired, '0');
		std::cout << "N=" << N << ", no_paired=" << no_paired << std::endl;

		str[0] = '(';

		if(0.0 < gsl_rng_uniform(r)){ //not interloop
			//arranging (s and )s
			int n_c=1, n_rc=0;
			std::vector<std::string::iterator> where;

			for(std::string::iterator ch=str.begin() + 1; ch!=str.end(); ++ch){
				//std::cout << "n_p: " << n_p << ", n_c: " << n_c << ", n_rc: " << n_rc << std::endl;
				//std::cout << ( (double) n_p-n_c)/( (double) n_p-n_rc) << std::endl;
				if(gsl_rng_uniform(r) < ( (double) n_p-n_c)/( (double) n_p-n_rc) ){
					*ch = '(';
					n_c++;
				}
				else{
					*ch = ')';
					n_rc++;
				}
			}
			//adding points to it
			int n_points = N - no_paired;
			//first adding loop bases
			for(std::string::iterator ch=str.begin(); (ch+1) != str.end(); ++ch){
				if(*ch == '(' && *(ch+1) == ')' ) where.push_back(ch+1);
			}
			for(; n_points > 0 && where.size() > 0 ; n_points-- ) {
				int which = gsl_rng_uniform_int(r, where.size());
				str.insert(where[which ], '.');
				where.erase(where.begin() + which);
			}
			//adding extra bases
			for(; n_points > 0; n_points--){
				str.insert(str.begin() + gsl_rng_uniform_int(r, str.length()-1) + 1, '.') ;
			}


		} 
		else { // interloop
		}

	}
	else {
		str.assign(N, '.');
		std::cout << "N=" << N << ", no_paired=" << no_paired << std::endl;
	}

	std::cout << str << std::endl;
















	//close rng
	gsl_rng_free(r);
	return 0;
}
