#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <boost/multiprecision/cpp_int.hpp>
#include <cmath>

#include "annot.h"
#include "dv_tools.h"
#include "randomgen.h"

using namespace std;


unsigned long long int strToInt(char *str, int length){
	int res = 0;
	for(; length--; str++) {
		if('.' != *str){
			res += (1 << length);
		}
	} 
	return(res);
}

double vmi(int y){
	return(y+10);
}


gsl_rng * r;

int main(int argc, char *argv[]) {

    //initialise rng
    time_t timer;
    r = (gsl_rng *) gsl_rng_alloc (gsl_rng_mt19937);
    gsl_rng_set(r, time(&timer));
/*
	char str[] = "..(...).\0", str2[] = "(...)\0";
	cout << strToInt(str, strlen(str)) << endl;
	cout << strToInt(str2, strlen(str2)) << endl;
	cout << "char " << sizeof(char) << " bool " << sizeof(bool) << " int " << sizeof(int) << " u l l i " << sizeof(unsigned long long int) << endl;
   	cout << sizeof(boost::multiprecision::uint256_t) << endl;
*/
/*
	std::ifstream file;
	std::string word, line;
	file.open("IN/mapping.txt");

	//while(file >> word) std::cout << word << std::endl << std::endl;

	file.seekg(0);

	while (std::getline(file, line)){
		std::istringstream linestream(line);
		std::cout << "new line" << std::endl;
		while(linestream >> word) std::cout << word << std::endl;
	} 
*/	
/*
	std::string p = "string";
	char ch[] = "ez nem egy string\0";

	std::strcpy(ch, p.c_str() );
	std::cout << ch << std::endl;
*/



/*	dv_annot::PatternPool pool;
	std::cout << "Created pool" << std::endl;
	pool.readFile(par_str_pool);
	std::cout << "So far so good..." << std::endl;
	std::cout << pool.rules.size() << std::endl;

	char seq[]="GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\0", str[]="((((((.........)))))(........)))))\0";
	std::cout << std::strlen(seq) << " " << std::strlen(str) << std::endl;
	int a = -1;
	a = pool.search(seq, str);
	std::cout << a << "db " << pool.a[0] << " " << pool.a[1] << " " << pool.a[2] << std::endl;


	//pool.printRules();
*/


/*
	dvtools::quickPosVals v(5, &vmi);
	std::cout << v[89] << std::endl;
	//v.f = &vmi;
*/

//   	 std::cout << gsl_rng_uniform_int(r,1 ) << std::endl;
//    std::cout << 5.8%1 << std::endl;

/*
    double w = 5.1, frac;
    frac =  w - (int) w;
    std::cout << frac << std::endl;
//    std::cout << std::modf(1.56, &NULL) << std::endl;
*/


	std::string x("ABCD");

//	std::cout << x.capacity() << std::endl;

//	x = "ABCD";
/*	std::cout << x.capacity() << std::endl;
	x.resize(3);
	x.reserve(200);
	x[3] = 'D';
	std::cout << x.capacity() << std::endl;
	x.resize(4);
	//x.fit

	std::cout << x << std::endl;

	std::cout << x.capacity() << std::endl;
*/
/*	std::string::reverse_iterator xp = x.rbegin();

	std::cout << *(xp++) << std::endl;
	std::cout << *xp << std::endl;
*/

	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;
	std::cout << gsl_rng_uniform_int(r, 2) << std::endl;


    //close rng
    gsl_rng_free(r);


	return 0;
}
/*

	0010 0010

	0001 0001
&	0000 0000

<< 1	0010 0010
&	0010 0010



*/

