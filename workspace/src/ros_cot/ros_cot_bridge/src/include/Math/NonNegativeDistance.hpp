#pragma once
#include "Distance.hpp"

namespace ARL {
	namespace Math {

		class NonNegativeDistance : public Distance < NonNegativeDouble > {
		public:
			NonNegativeDistance(const NonNegativeDouble& d = NonNegativeDouble::NaN(), LengthUnits u = Meters) { setValue(d, u); }
			NonNegativeDistance(const DOMElement* pElement) : Distance< NonNegativeDouble >(pElement) { }
			static NonNegativeDistance NotADistance() { return NonNegativeDistance(static_cast<double>(std::numeric_limits<double>::quiet_NaN()), Meters); }
			operator Distance<>() const { return Distance<>(this->getValueInPreferredUnits().getValue(), this->getPreferredUnits()); }
		protected:
			friend NonNegativeDistance operator+(const NonNegativeDistance& s1, const NonNegativeDistance& s2) {
				return NonNegativeDistance(s1.magnitude + s2.magnitude, s1.units); // since Speed is always stored internally in MetersPerSecond, adding two speeds is always simple. Take preferred units as the left hand side.
			}
			friend NonNegativeDistance operator-(const NonNegativeDistance& s1, const NonNegativeDistance& s2) {
				if (s1.magnitude < s2.magnitude) throw Exception("Cannot subtract larger NonNegativeDistance from Smaller - result is negative.");
				return NonNegativeDistance(s1.magnitude - s2.magnitude, s1.units);
			}
			friend NonNegativeDistance& operator+=(NonNegativeDistance& s1, const NonNegativeDistance& s2) {
				s1.magnitude += s2.magnitude;
				return s1;
			}
			friend NonNegativeDistance& operator-=(NonNegativeDistance& s1, const NonNegativeDistance& s2) {
				if (s1.magnitude < s2.magnitude) throw Exception("Cannot subtract larger NonNegativeDistance from Smaller - result is negative.");
				s1.magnitude -= s2.magnitude;
				return s1;
			}
			friend NonNegativeDistance operator*(const NonNegativeDistance& s, double a){
				if (a < 0) throw Exception("Cannot multiply a NonNegativeDistance by a negative number - result is negative distance.");
				return NonNegativeDistance(s.magnitude*a, s.units);
			}
			friend NonNegativeDistance operator*(double a, const NonNegativeDistance& s){
				if (a < 0) throw Exception("Cannot multiply a NonNegativeDistance by a negative number - result is negative distance.");
				return NonNegativeDistance(s.magnitude*a, s.units);
			}
			friend NonNegativeDistance operator/(const NonNegativeDistance& s, double a){
				if (a < 0) throw Exception("Cannot divide a NonNegativeDistance by a negative number - result is negative distance.");
				return NonNegativeDistance(s.magnitude / a, s.units);
			}

			friend class cereal::access;
		};

	}
}