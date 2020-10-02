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
			double *vals;
			int max;
			double (*f)(int);

			//Constructor
			quickPosVals(int _max, double (*_f)(int)) : max{_max}, f{_f}{
				if(max > -1) {
					vals = new double [max + 1];
					for(int i = 0; i <= max; i++){
						vals[i] = f(i);
					}
				}

			}

			//Copy Constructor
			quickPosVals(const quickPosVals &obj){
				max = obj.max;
				f = obj.f;

				if(max > -1) {
					vals = new double [max + 1];
					for(int i = 0; i <= max; i++){
						vals[i] = obj.vals[i];
					}
				}
			}

			//Destructor
			~quickPosVals(){
				if(max > -1) delete [] (vals);
			}

			double &operator[](int i){
				if(i <= max) return(vals[i]);
				else return (0.0);
			}

			//calculate for the given range
			void setMax(int newmax){
				if(max != newmax){
					if(max > -1) delete [] (vals);
					max = newmax;
					if (max > -1) {
						vals = new double (max + 1);
						for(int i = 0; i <= max; i++){
							vals[i] = f(i);
						}
					}
				}
			}

			//get max
			int getMax() {return max;}

			
	};

}

#endif
