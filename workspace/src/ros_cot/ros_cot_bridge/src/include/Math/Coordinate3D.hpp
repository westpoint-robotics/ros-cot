#pragma once
#include "ScaledValue.hpp"
#include "Units.hpp"
#include "Point2D.hpp"
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <cereal/access.hpp>
#include <Utility/DOMHelper.hpp>

namespace ARL {
	namespace Math {

		class Coordinate3D : public ScaledValue < Point3D, LengthUnits > {
		public:
			Coordinate3D(double x = 0, double y = 0, double z = 0, LengthUnits u = Meters) {
				setValue(Point3D(x, y, z), u);
			}
			Coordinate3D(const Point3D& value, LengthUnits u = Meters) {
				setValue(value, u);
			}
			void setValue(const Point3D& v, LengthUnits u = Meters) {
				units = u; //preferred units
				switch (u) { //always store internally in Meters
				case Meters: magnitude = v; break;
				case NauticalMiles: magnitude = nauticalMilesToMeters(v); break;
				case Miles: magnitude = milesToMeters(v); break;
				case Kilometers: magnitude = 1000 * v; break;
				case Feet: magnitude = feetToMeters(v); break;
				case Yards: magnitude = yardsToMeters(v); break;
				default: throw Exception("unknown distance units in Coordinate3D::setValue()");
				}
			}

			Coordinate3D normalize() { magnitude.normalize(); return *this; }

			Point3D getValue(const LengthUnits u = Meters) const override {
				switch (u)  {
				case Meters: return magnitude;
				case NauticalMiles: return metersToNauticalMiles(magnitude);
				case Miles: return metersToMiles(magnitude);
				case Kilometers: return 0.001 * magnitude;
				case Feet: return metersToFeet(magnitude);
				case Yards: return metersToYards(magnitude);
				default: throw Exception("unknown distance units in Coordinate3D::getValue()");
				}
			}

			Distance<> X() const { return Distance<>(getValueInPreferredUnits().x, units); }
			Distance<> Y() const { return Distance<>(getValueInPreferredUnits().y, units); }
			Distance<> Z() const { return Distance<>(getValueInPreferredUnits().z, units); }

//			operator Coordinate2D() const {
//				auto v = getValueInPreferredUnits();
//				return Coordinate2D(v.x, v.y, units);
//			}

			DOMElement* createDOMElement(DOMDocument* pDoc, const XMLCh* name = xStr(L"Coordinate3D")) const override {
				DOMElement* pElement = magnitude.createDOMElement(pDoc, name);
				switch (units) {
				case Meters: pElement->setAttribute(xStr(L"Units"), xStr(L"Meters")); break;
				case NauticalMiles: pElement->setAttribute(xStr(L"Units"), xStr(L"NauticalMiles")); break;
				case Miles: pElement->setAttribute(xStr(L"Units"), xStr(L"Miles")); break;
				case Kilometers: pElement->setAttribute(xStr(L"Units"), xStr(L"Kilometers")); break;
				case Feet: pElement->setAttribute(xStr(L"Units"), xStr(L"Feet")); break;
				case Yards: pElement->setAttribute(xStr(L"Units"), xStr(L"Yards")); break;
				default: throw Exception("Unknown Length Units in Coordinate3D::createDOMElement()");
				}
				return pElement;
			}

			Coordinate3D& filteredUpdate(const Coordinate3D& p, const double alpha = 1.0 / 4) {
				magnitude.filteredUpdate(p.magnitude, alpha);
				return *this;
			}

			friend Coordinate3D operator+(const Coordinate3D& c1, const Coordinate3D& c2) {
				Coordinate3D result = c1;
				result.magnitude += c2.magnitude;
				return result;
			}

			friend Coordinate3D operator-(const Coordinate3D& c1, const Coordinate3D& c2) {
				Coordinate3D result = c1;
				result.magnitude -= c2.magnitude;
				return result;
			}

			friend Coordinate3D operator*(const Coordinate3D& c, const double x) {
				Coordinate3D result = c;
				result.magnitude *= x;
				return result;
			}

			friend Coordinate3D operator*(const double x, const Coordinate3D& c) {
				return c * x;
			}

			friend ostream& operator<<(ostream& o, const Coordinate3D& c) {
				auto mag = c.getValue(Meters);
				o << mag << " Meters";
				return o;
			}

			friend class cereal::access;
		};

	}
}