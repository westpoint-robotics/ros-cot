#pragma once
#include "ScaledValue.hpp"
#include "Units.hpp"
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <cereal/access.hpp>
#include <Utility/DOMHelper.hpp>
#include <Utility/StringConversions.hpp>

namespace ARL {
	namespace Math {

		using XERCES_CPP_NAMESPACE::DOMElement;
		using XERCES_CPP_NAMESPACE::DOMDocument;
		using ARL::Utility::xStr;
		using ARL::Utility::quickW28;
		using namespace std;

		template <class T = double>
		class Distance : public ScaledValue < T, LengthUnits > {
		public:
			Distance(const T& d = std::numeric_limits<T>::quiet_NaN(), LengthUnits u = Meters) { setValue(d, u); }

			///construct a Distance instance from the DOM Element; magnitude and units are child DOM Elements.
			Distance(const DOMElement* pElement) {
				magnitude = ARL::Utility::DOMHelper::parseDOMElement<T>(pElement);
				units = ARL::Utility::DOMHelper::parseDOMAttribute<LengthUnits>(pElement, xStr("Units"));
				setValue(magnitude, units);
			}

			void setValue(const T& v, LengthUnits u = Meters) { //always store internally in Meters, preferred units stored in Units
				this->units = u;
				switch (u) {
				case Meters: this->magnitude = v; break;
				case NauticalMiles: this->magnitude = nauticalMilesToMeters(v); break;
				case Miles: this->magnitude = milesToMeters(v); break;
				case Kilometers: this->magnitude = 1000 * v; break;
				case Feet: this->magnitude = feetToMeters(v); break;
				case Yards: this->magnitude = yardsToMeters(v); break;
				default: throw Exception("Unknown LengthUnits in Distance::setValue()");
				}
			}

			void filteredUpdate(const Distance& v, const double alpha = 1.0 / 3) {
				this->magnitude = InfiniteImpulseResponseDigitalFilter<T>::computeResponse(v.getValue(Meters), getValue(Meters), alpha);
			}

			T getValue(const LengthUnits u = Meters) const override {
				switch (u)  {
				case Meters: return this->magnitude;
				case NauticalMiles: return metersToNauticalMiles(this->magnitude / 3600);
				case Miles: return metersToMiles(this->magnitude / 3600);
				case Kilometers: return this->magnitude / 1000;
				case Feet: return metersToFeet(this->magnitude);
				case Yards: return metersToYards(this->magnitude);
				default: throw Exception("unknown length units in Distance::getValue()");
				}
			}
			Distance operator-() const { 
				Distance d;
				d.magnitude = -this->magnitude; d.units = this->units;
				return d;
			}

			friend Distance operator+(const Distance& s1, const Distance& s2) {
				return Distance(s1.magnitude + s2.magnitude, s1.units); // since Speed is always stored internally in MetersPerSecond, adding two speeds is always simple. Take preferred units as the left hand side.
			}
			friend Distance operator-(const Distance& s1, const Distance& s2) {
				return Distance(s1.magnitude - s2.magnitude, s1.units);
			}
			friend Distance& operator+=(Distance& s1, const Distance& s2) {
				s1.magnitude += s2.magnitude;
				return s1;
			}
			friend Distance& operator-=(Distance& s1, const Distance& s2) {
				s1.magnitude -= s2.magnitude;
				return s1;
			}
			friend Distance operator*(const Distance& s, double a) {
				return Distance(s.magnitude*a, s.units);
			}
			friend Distance operator*(double a, const Distance& s) {
				return Distance(s.magnitude*a, s.units);
			}
			friend Distance operator/(const Distance& s, double a) {
				return Distance(s.magnitude / a, s.units);
			}
			friend double operator/(const Distance& s, const Distance& a){
				return s.magnitude / a.magnitude;
			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Distance")) const {
				DOMElement* pElement = ARL::Utility::DOMHelper::createDOMElement(getValueInPreferredUnits(), pDoc, name);
				return ARL::Utility::DOMHelper::setDOMAttribute(units, pElement, xStr("Units"));
			}

			static Distance parseDOMAttribute(const DOMElement* pElement, const XMLCh* name = L"Distance") {
				return Distance(pElement, name);
			}

			static Distance NotADistance() { return Distance(static_cast<T>(std::numeric_limits<T>::quiet_NaN()), Meters); }

			friend class cereal::access;
		};

	}
}