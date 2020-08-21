#ifndef _DV_TOOLS_
#define _DV_TOOLS_

namespace dvtools {

inline int Rmod(int a, int b) {
	int r = a%b; 
	return r >= 0 ? r : r + b;
}

int brokenStickVals(double *values, int noChoices, double sum, double random); 


}

#endif
