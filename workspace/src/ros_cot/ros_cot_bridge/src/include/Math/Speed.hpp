#pragma once

#include "ScaledValue.hpp"
#include "Units.hpp"
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <cereal/access.hpp>
#include <Utility/DOMHelper.hpp>

namespace ARL {
	namespace Math {

		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMDocument;

		template <class T = double>
		class Speed : public ScaledValue < T, SpeedUnits > {
		public:
			Speed() {}
			Speed(const T& v, SpeedUnits u = MetersPerSecond) {
				setValue(v, u);
			}
			Speed(const DOMElement* pElement) : T(pElement) {
				units = ARL::Utility::DOMHelper::parseDOMAttribute(pElement, xStr("units"));
				setValue(magnitude, units);
			}
			void setValue(const T& v, SpeedUnits u = MetersPerSecond) { //always store internally in MetersPerSecond, preferred units stored in Units
				this->units = u;
				switch (u) {
				case MetersPerSecond: this->magnitude = v; break;
				case Knots: this->magnitude = nauticalMilesToMeters(v / 3600); break;
				case FeetPerSecond: this->magnitude = feetToMeters(v); break;
				case MilesPerHour: this->magnitude = milesToMeters(v / 3600); break;
				case KilometersPerHour: this->magnitude = 1000 * v / 3600; break;
				default: throw Exception("unknown distance units in Speed::setValue()");
				}
			}
			T getValue(const SpeedUnits u = MetersPerSecond) const override {
				switch (u)  {
				case MetersPerSecond: return this->magnitude;
				case Knots: return metersToNauticalMiles(this->magnitude * 3600);
				case FeetPerSecond: return metersToFeet(this->magnitude);
				case MilesPerHour: return metersToMiles(this->magnitude * 3600);
				case KilometersPerHour: return this->magnitude * 3600000;
				default: throw Exception("unknown distance units in Speed::getValue()");
				}
			}
			void abs() { if (this->magnitude < 0) this->magnitude = -this->magnitude; }

			Speed operator-() const { return Speed(-this->magnitude, this->units); }
			friend Speed operator+(const Speed& s1, const Speed& s2) {
				return Speed(s1.magnitude + s2.magnitude, MetersPerSecond); // since Speed is always stored internally in MetersPerSecond, adding two speeds is always simple. Take preferred units as the left hand side.
			}
			friend Speed operator-(const Speed& s1, const Speed& s2) {
				return Speed(s1.magnitude - s2.magnitude, MetersPerSecond);
			}
			friend Speed& operator+=(Speed& s1, const Speed& s2) {
				s1.magnitude += s2.magnitude;
				return s1;
			}
			friend Speed& operator-=(Speed& s1, const Speed& s2) {
				s1.magnitude -= s2.magnitude;
				return s1;
			}
			friend Speed operator*(const Speed& s, double a){
				return Speed(s.magnitude*a, s.units); //ok as long as linear relations
			}
			friend Speed operator*(double a, const Speed& s){
				return Speed(s.magnitude*a, s.units); //ok as long as linear relations
			}
			friend Speed operator/(const Speed& s, double a){
				return Speed(s.magnitude / a, s.units); //ok as long as linear relations
			}
			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Speed")) const override {
				return ARL::Utility::DOMHelper::setDOMAttribute(units, ARL::Utility::DOMHelper::createDOMElement(magnitude, pDoc, name), xStr("units"));
			}

			friend class cereal::access;
		};
	}
}