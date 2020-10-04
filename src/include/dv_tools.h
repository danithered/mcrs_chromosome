#ifndef _DV_TOOLS_
#define _DV_TOOLS_

namespace dvtools {

	inline int Rmod(int a, int b) {
		int r = a%b; 
		return r >= 0 ? r : r + b;
	}

	int brokenStickVals(double *values, int noChoices, double sum, double random); 

	class quickPosVals {
		public:
			double last;

			//Constructor
			quickPosVals(int _max, double (*_f)(int));

			//Copy Constructor
			quickPosVals(const quickPosVals &obj);

			//Destructor
			~quickPosVals();

			//indexing
			double &operator[](int i){
				if(i <= max) last = vals[i];
				else last = f(i);
				return last;
			}

			//calculate for the given range
			void setMax(int newmax);

			//get max
			int getMax();

		private:
			double *vals;
			int max;
			double (*f)(int);
			
	};

}

#endif
