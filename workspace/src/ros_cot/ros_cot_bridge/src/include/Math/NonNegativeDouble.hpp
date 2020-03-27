#pragma once

#include <Utility/DOMHelper.hpp>
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <limits>
#include <cereal/access.hpp>

namespace ARL {
	namespace Math {
		using XERCES_CPP_NAMESPACE::DOMElement;

		class NonNegativeDouble {
		public:
			NonNegativeDouble(const int d) { setValue(d); }
			NonNegativeDouble(const double d = 0) { setValue(d); }
			NonNegativeDouble(const DOMElement* pElement) {
				setValue(ARL::Utility::DOMHelper::parseDOMElement<double>(pElement));
			}
			void setValue(const double d) {
				if (d < 0) throw Exception("A NonNegativeDouble may not be assigned negative values");
				value = d;
			}
			double getValue() const { return value; }
			explicit operator double() const { return value; }
			static NonNegativeDouble NaN() { return std::numeric_limits<double>::quiet_NaN(); }
		protected:
			double value;

			friend class cereal::access;

			template <class Archive>
			void serialize(Archive & archive) {
				archive(value);
			}

			friend NonNegativeDouble operator+(const NonNegativeDouble& s1, const NonNegativeDouble& s2) {
				return NonNegativeDouble(s1.value + s2.value);
			}
			friend NonNegativeDouble operator-(const NonNegativeDouble& s1, const NonNegativeDouble& s2) {
				if (s1 < s2)throw Exception("Cannot subtract a larger NonNegativeDouble from a smaller one - the result is negative.");
				return NonNegativeDouble(s1.value - s2.value);
			}
			friend NonNegativeDouble& operator+=(NonNegativeDouble& s1, const NonNegativeDouble& s2) {
				s1.value += s2.value;
				return s1;
			}
			friend NonNegativeDouble& operator-=(NonNegativeDouble& s1, const NonNegativeDouble& s2) {
				if (s1 < s2) throw Exception("Cannot subtract a larger NonNegativeDouble from a smaller one - the result is negative.");
				s1.value -= s2.value;
				return s1;
			}
			friend NonNegativeDouble operator*(const NonNegativeDouble& s, double a){
				if (a < 0) throw Exception("Cannot multiply a NonNegativeDouble by a negative number - result is negative distance.");
				return NonNegativeDouble(s.value*a);
			}
			friend NonNegativeDouble operator*(double a, const NonNegativeDouble& s){
				if (a < 0) throw Exception("Cannot multiply a NonNegativeDouble by a negative number - result is negative distance.");
				return NonNegativeDouble(s.value*a);
			}
			friend NonNegativeDouble operator/(const NonNegativeDouble& s, double a){
				if (a < 0) throw Exception("Cannot divide a NonNegativeDouble by a negative number - result is negative distance.");
				return NonNegativeDouble(s.value / a);
			}

			friend bool operator<(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value < d2.value;
			}
			friend bool operator>(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value > d2.value;
			}
			friend bool operator==(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value == d2.value;
			}
			friend bool operator!=(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value != d2.value;
			}
			friend bool operator<=(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value <= d2.value;
			}
			friend bool operator>=(const NonNegativeDouble& d1, const NonNegativeDouble& d2) {
				return d1.value >= d2.value;
			}

			friend std::ostream& operator<<(std::ostream& o, const NonNegativeDouble& d) {
				o << d.value;
				return o;
			}
			friend std::wostream& operator<<(std::wostream& o, const NonNegativeDouble& d) {
				o << d.value;
				return o;
			}
			friend std::wistream& operator>>(std::wistream& i, NonNegativeDouble& d) {
				double v;
				i >> v;
				d.setValue(v);
				return i;
			}
			friend std::istream& operator>>(std::istream& i, NonNegativeDouble& d) {
				double v;
				i >> v;
				d.setValue(v);
				return i;
			}
		};

	}
}