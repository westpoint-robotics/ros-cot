#pragma once
#include <cmath>

namespace ARL {
	namespace Math {
		
		using namespace std;

		template <class T = double>
		class LogisticFunction {
		public:
			///@param fmax defines of the range of the transfer function (-fmax, fmax)
			///@param ratio defines x1 = ratio * fmax, therefore -k = log(0.5) / x1 = log(0.5) / (ratio * fmax)
			LogisticFunction(const T& fmax, const double& ratio) : fmax(2*fmax), minusK(log(0.5) / (ratio * fmax)) {
				assert(this->fmax > 0);
				assert(ratio > 0.001); //avoid discontinuity as ratio -> 0
			}
			T operator()(const T& x) {
				return fmax * (1 / (1 + exp(minusK * x)) - 0.5);
			}
		protected:
			T fmax;
			T minusK;
		};
	}
}