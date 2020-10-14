#include "rnarep.h" 


namespace rnarep {
	char bases[6] = "GCUAN";

	double EoptCalc(int n){
		return (double) n;
	}

	double cvalCalc(int n){
		return (double) n;
	}

	double length_depCalc(int n){
		return (double) n;
	}

	dvtools::quickPosVals CellContent::Eopt(100, &EoptCalc);
	dvtools::quickPosVals CellContent::cval(100, &cvalCalc);
	dvtools::quickPosVals CellContent::length_dep(100, &length_depCalc);

	dv_annot::PatternPool CellContent::patterns;
}

