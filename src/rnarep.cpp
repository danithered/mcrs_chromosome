#include "rnarep.h" 


namespace rnarep {
	char bases[6] = "GCUAN";

	double EoptCalc(int n){  //fogalmam sincs ezt hogy kalkulaljam ki :(
		return (double) n;
	}

	/* Pfold = 1 / ( 1-exp(c*mfe) )
	 * Pfold = 1 in case of mfe = Eopt
	 * 1 = 1 - exp(c * Eopt)
	 *
	 * */
	double cvalCalc(int n){
		return (double) n; //cant calculate if there is no Eopt
	}

	double length_depCalc(int L){
		return par_g / (par_b1 + par_b2 * L);
	}

	double m_sigmaCalc(int m){
		return std::pow( (double) m, par_sigma);
	}

	dvtools::quickPosVals CellContent::Eopt(100, &EoptCalc);
	dvtools::quickPosVals CellContent::cval(100, &cvalCalc);
	dvtools::quickPosVals CellContent::length_dep(100, &length_depCalc);
	dvtools::quickPosVals CellContent::m_sigma(100, &m_sigmaCalc);

	dv_annot::PatternPool CellContent::patterns;
}

