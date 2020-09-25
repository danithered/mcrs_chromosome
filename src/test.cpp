#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <boost/multiprecision/cpp_int.hpp>

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

int main(int argc, char *argv[]) {
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

	std::string p = "string";
	char ch[] = "ez nem egy string\0";

	std::strcpy(ch, p.c_str() );
	std::cout << ch << std::endl;


	return 0;
}
/*

	0010 0010

	0001 0001
&	0000 0000

<< 1	0010 0010
&	0010 0010



*/

